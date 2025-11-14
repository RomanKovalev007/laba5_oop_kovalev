#pragma once

#include <memory_resource>
#include <cstddef>
#include <utility>
#include <iterator>


namespace pmr = std::pmr;

template <typename T>
class LinkedList {
private:
    struct Node {
        T value;
        Node* next;
        template <typename... Args>
        Node(Args&&... args) : value(std::forward<Args>(args)...), next(nullptr) {}
    };

    using AllocNode = pmr::polymorphic_allocator<Node>;
    using AllocTraits = std::allocator_traits<AllocNode>;

    
    AllocNode alloc_;
    Node* head_;
    Node* tail_;
    std::size_t sz_;

public:
    explicit LinkedList(pmr::memory_resource* mr = pmr::get_default_resource())
        : alloc_(mr), head_(nullptr), tail_(nullptr), sz_(0) {}

    ~LinkedList() {
        clear();
    }

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    LinkedList(LinkedList&& other) noexcept
        : alloc_(other.alloc_), head_(other.head_), tail_(other.tail_), sz_(other.sz_){
        other.head_ = other.tail_ = nullptr;
        other.sz_ = 0;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            alloc_ = other.alloc_;
            head_ = other.head_;
            tail_ = other.tail_;
            sz_ = other.sz_;
            other.head_ = other.tail_ = nullptr;
            other.sz_ = 0;
        }
        return *this;
    }

    template <typename... Args>
    void push_front(Args&&... args) {
        Node* node = AllocTraits::allocate(alloc_, 1);
        try {
            AllocTraits::construct(alloc_, node, std::forward<Args>(args)...);
        } catch (...) {
            AllocTraits::deallocate(alloc_, node, 1);
            throw;
        }
        node->next = head_;
        head_ = node;
        if (!tail_) tail_ = node;
        ++sz_;
    }

    template <typename... Args>
    void push_back(Args&&... args) {
        Node* node = AllocTraits::allocate(alloc_, 1);
        try {
            AllocTraits::construct(alloc_, node, std::forward<Args>(args)...);
        } catch (...) {
            AllocTraits::deallocate(alloc_, node, 1);
            throw;
        }
        node->next = nullptr;
        if (tail_) tail_->next = node;
        tail_ = node;
        if (!head_) head_ = node;
        ++sz_;
    }

    void pop_front() {
        if (!head_) return;
        Node* old = head_;
        head_ = head_->next;
        if (!head_) tail_ = nullptr;
        AllocTraits::destroy(alloc_, old);
        AllocTraits::deallocate(alloc_, old, 1);
        --sz_;
    }

    void clear() {
        while (head_) {
            Node* next = head_->next;
            AllocTraits::destroy(alloc_, head_);
            AllocTraits::deallocate(alloc_, head_, 1);
            head_ = next;
        }
        head_ = tail_ = nullptr;
        sz_ = 0;
    }

    bool empty() const noexcept { return sz_ == 0; }
    std::size_t size() const noexcept { return sz_; }

    class iterator{
        Node *cur_ = nullptr;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        iterator() noexcept = default;
        explicit iterator(Node *p) noexcept : cur_(p) {}

        reference operator*() const noexcept { return cur_->value; }
        pointer operator->() const noexcept { return std::addressof(cur_->value); }

        iterator &operator++() noexcept
        {
            cur_ = cur_->next;
            return *this;
        }
        iterator operator++(int) noexcept
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        friend bool operator==(const iterator &a, const iterator &b) noexcept { return a.cur_ == b.cur_; }
        friend bool operator!=(const iterator &a, const iterator &b) noexcept { return a.cur_ != b.cur_; }
    };

    iterator begin() noexcept { return iterator(head_); }
    iterator end() noexcept { return iterator(nullptr); }
};
