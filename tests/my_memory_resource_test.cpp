#include <gtest/gtest.h>
#include <memory_resource>
#include <string>

#include "../include/my_memory_resource.h"
#include "../include/linked_list.h"

TEST(MyMemoryResourceTest, BasicAllocDealloc) {
    MyMemoryResource mem(1 << 20);
    void* ptr = mem.allocate(64);
    EXPECT_NE(ptr, nullptr);

    mem.deallocate(ptr, 64);
}

TEST(MyMemoryResourceTest, MultiAlloc) {
    MyMemoryResource mem(1 << 20);
    void* ptr1 = mem.allocate(32);
    void* ptr2 = mem.allocate(64);
    void* ptr3 = mem.allocate(128);
    
    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_NE(ptr3, nullptr);
    
    EXPECT_NE(ptr1, ptr2);
    EXPECT_NE(ptr1, ptr3);
    EXPECT_NE(ptr2, ptr3);
    
    mem.deallocate(ptr1, 32);
    mem.deallocate(ptr2, 64);
    mem.deallocate(ptr3, 128);
}

TEST(MyMemoryResourceTest, OutOfMemory) {
    MyMemoryResource mr(1024);
    EXPECT_THROW((void)mr.allocate(2048), std::bad_alloc);
}

TEST(MyMemoryResourceTest, Alignment) {
    MyMemoryResource mem(1 << 20);
    void* ptr1 = mem.allocate(1, 16);
    EXPECT_NE(ptr1, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr1) % 16, 0);
    
    void* ptr2 = mem.allocate(1, 32);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr2) % 32, 0);
    
    mem.deallocate(ptr1, 1, 16);
    mem.deallocate(ptr2, 1, 32);
}

TEST(MyMemoryResourceTest, Coalescing) {
    MyMemoryResource mem(1 << 20);

    void* ptr1 = mem.allocate(64);
    void* ptr2 = mem.allocate(64);
    void* ptr3 = mem.allocate(64);
    
    mem.deallocate(ptr2, 64);
    mem.deallocate(ptr1, 64);
    mem.deallocate(ptr3, 64);
    
    void* large_ptr = mem.allocate(192);
    EXPECT_NE(large_ptr, nullptr);
    mem.deallocate(large_ptr, 192);
}