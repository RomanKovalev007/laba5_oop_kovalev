#pragma once

#include <memory_resource>
#include <map>
#include <cstdint>
#include <new>


class MyMemoryResource : public std::pmr::memory_resource {
public:
    MyMemoryResource(std::size_t total_size): total_size_(total_size){
        base_ = ::operator new(total_size_, std::nothrow);
        if (!base_) throw std::bad_alloc();
        std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(base_);
        free_blocks_.emplace(addr, total_size_);
    }

    ~MyMemoryResource() override {
        ::operator delete(base_);
    }

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        for (auto it = free_blocks_.begin(); it != free_blocks_.end(); ++it) {
            std::uintptr_t block_addr = it->first;
            std::size_t block_size = it->second;

            std::uintptr_t aligned = align_up(block_addr, alignment);
            std::size_t offset = static_cast<std::size_t>(aligned - block_addr);
            if (block_size < offset) continue;
            std::size_t remaining = block_size - offset;
            if (remaining < bytes) continue;

            void* result = reinterpret_cast<void*>(aligned);

            std::uintptr_t old_start = block_addr;
            std::size_t old_size = block_size;
            free_blocks_.erase(it);

            if (offset > 0) {
                free_blocks_.emplace(old_start, offset);
            }

            std::uintptr_t right_start = aligned + bytes;
            std::size_t right_size = old_size - offset - bytes;
            if (right_size > 0) {
                free_blocks_.emplace(right_start, right_size);
            }

            allocated_blocks_.emplace(reinterpret_cast<std::uintptr_t>(result), bytes);

            return result;
        }

        throw std::bad_alloc();
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        if (!p) return;
        std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(p);

        auto it_alloc = allocated_blocks_.find(addr);
        if (it_alloc != allocated_blocks_.end()) {
            bytes = it_alloc->second;
            allocated_blocks_.erase(it_alloc);
        }

        coalesce_insert(addr, bytes);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:
    static std::uintptr_t align_up(std::uintptr_t p, std::size_t alignment) {
        std::uintptr_t a = alignment;
        if (a == 0) return p;
        std::uintptr_t rem = p % a;
        if (rem == 0) return p;
        return p + (a - rem);
    }

    void coalesce_insert(std::uintptr_t addr, std::size_t size) {
        std::uintptr_t start = addr;
        std::uintptr_t end = addr + size;

        auto it = free_blocks_.lower_bound(start);

        if (it != free_blocks_.begin()) {
            auto prev = std::prev(it);
            std::uintptr_t prev_start = prev->first;
            std::size_t prev_size = prev->second;
            if (prev_start + prev_size == start) {
                start = prev_start;
                size += prev_size;
                it = free_blocks_.erase(prev);
            }
        }


        while (it != free_blocks_.end()) {
            std::uintptr_t cur_start = it->first;
            std::size_t cur_size = it->second;
            if (cur_start == end) {
                size += cur_size;
                end = start + size;
                it = free_blocks_.erase(it);
            } else {
                break;
            }
        }

        free_blocks_.emplace(start, size);
    }

    void* base_{nullptr};
    std::size_t total_size_{0};

    std::map<std::uintptr_t, std::size_t> free_blocks_;
    std::map<std::uintptr_t, std::size_t> allocated_blocks_;
};