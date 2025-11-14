#include <gtest/gtest.h>
#include <string>
#include "../include/my_memory_resource.h"
#include "../include/linked_list.h"

namespace pmr = std::pmr;

struct Person {
    int id;
    pmr::string name;
    
    Person(int i, pmr::string n) : id(i), name(move(n)) {}
    
    bool operator==(const Person& other) const {
        return id == other.id && name == other.name;
    }
};

TEST(LinkedListTest, Consruct) {
    MyMemoryResource mem(1 << 20);
    LinkedList<int> lst{&mem};

    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0u);
}

TEST(LinkedListTest, Push) {
    MyMemoryResource mem(1 << 20);

    pmr::string a{&mem}; a = "Roma";
    pmr::string b{&mem}; b = "Kolya";
    pmr::string c{&mem}; c = "Leonid";

    LinkedList<Person> lst{&mem};
    lst.push_back(Person{1, move(a)});
    lst.push_back(Person{2, move(b)});
    lst.push_front(Person{0, move(c)});

    EXPECT_FALSE(lst.empty());
    EXPECT_EQ(lst.size(), 3u);
}

TEST(LinkedListTest, Pop) {
    MyMemoryResource mem(1 << 20);

    pmr::string a{&mem}; a = "Roma";
    pmr::string b{&mem}; b = "Kolya";
    pmr::string c{&mem}; c = "Leonid";

    LinkedList<Person> lst{&mem};
    lst.push_back(Person{1, move(a)});
    lst.push_back(Person{2, move(b)});
    lst.push_front(Person{0, move(c)});

    lst.pop_front();
    EXPECT_EQ(lst.size(), 2u);
}

TEST(LinkedListTest, Order) {
    MyMemoryResource mem(1 << 20);

    pmr::string a{&mem}; a = "Roma";
    pmr::string b{&mem}; b = "Kolya";
    pmr::string c{&mem}; c = "Leonid";

    LinkedList<Person> lst{&mem};
    lst.push_back(Person{1, move(a)});
    lst.push_back(Person{2, move(b)});
    lst.push_front(Person{0, move(c)});

    auto i = lst.begin();
    EXPECT_EQ(i->id, 0);
    EXPECT_EQ(i->name, "Leonid");
    ++i;
    EXPECT_EQ(i->id, 1);
    EXPECT_EQ(i->name, "Roma");
    ++i;
    EXPECT_EQ(i->id, 2);
    EXPECT_EQ(i->name, "Kolya");
    ++i;
    EXPECT_EQ(i, lst.end());
}

TEST(LinkedListTest, Move) {
    MyMemoryResource mem(1 << 20);
    
    LinkedList<int> lst1{&mem};

    for (int i = 0; i < 3; ++i){
        lst1.push_back(i + 1);
    }

    LinkedList<int> lst2{std::move(lst1)};

    EXPECT_TRUE(lst1.empty());
    EXPECT_EQ(lst1.size(), 0u);
    
    EXPECT_FALSE(lst2.empty());
    EXPECT_EQ(lst2.size(), 3u);

    auto i = lst2.begin();
    EXPECT_EQ(*i, 1);
    ++i;
    EXPECT_EQ(*i, 2);
    ++i;
    EXPECT_EQ(*i, 3);
}

TEST(LinkedListTest, Clear) {
    MyMemoryResource mem(1 << 20);
    LinkedList<int> lst{&mem};

    for (int i = 0; i < 10; ++i) {
        lst.push_back(i);
    }

    EXPECT_EQ(lst.size(), 10u);
    lst.clear();
    EXPECT_TRUE(lst.empty());
    EXPECT_EQ(lst.size(), 0u);

    lst.push_back(42);
    EXPECT_EQ(lst.size(), 1u);
    EXPECT_EQ(*lst.begin(), 42);
}

TEST(LinkedListTest, StringOperations) {
    MyMemoryResource mem(1 << 20);
    LinkedList<pmr::string> lst{&mem};

    pmr::string a{&mem}; a = "Hello";
    pmr::string b{&mem}; b = "World";
    pmr::string c{&mem}; c = "Test";

    lst.push_back(move(a));
    lst.push_back(move(b));
    lst.push_front(move(c));

    EXPECT_EQ(lst.size(), 3u);

    auto i = lst.begin();
    EXPECT_EQ(*i, "Test");
    ++i;
    EXPECT_EQ(*i, "Hello");
    ++i;
    EXPECT_EQ(*i, "World");
}

TEST(LinkedListTest, PersonOperations) {
    MyMemoryResource mem(1 << 20);
    LinkedList<Person> lst{&mem};

    lst.push_back(1, "oleg");
    lst.push_back(2, "matvei");
    lst.push_front(0, "max");

    EXPECT_EQ(lst.size(), 3u);

    auto i = lst.begin();
    EXPECT_EQ(i->id, 0);
    EXPECT_EQ(i->name, "max");
    i++;
    EXPECT_EQ(i->id, 1);
    EXPECT_EQ(i->name, "oleg");
    i++;
    EXPECT_EQ(i->id, 2);
    EXPECT_EQ(i->name, "matvei");
}

TEST(LinkedListTest, LargeList) {
    MyMemoryResource mem(1 << 20);
    LinkedList<int> lst{&mem};

    const int n = 1000;
    for (int i = 0; i < n; ++i) {
        lst.push_back(i);
    }

    EXPECT_EQ(lst.size(), n);

    int expected = 0;
    for (const auto& item : lst) {
        EXPECT_EQ(item, expected);
        expected++;
    }
}

TEST(LinkedListTest, PopFrontEmpty) {
    MyMemoryResource mem(1 << 20);
    LinkedList<int> lst{&mem};

    lst.pop_front();
    EXPECT_TRUE(lst.empty());

    lst.push_back(1);
    lst.pop_front();
    EXPECT_TRUE(lst.empty());
    
    lst.pop_front();
    EXPECT_TRUE(lst.empty());
}


TEST(LinkedListTest, MemoryReuse) {
    MyMemoryResource mem(1 << 20);
    LinkedList<int> lst{&mem};

    for (int i = 0; i < 100; ++i) {
        lst.push_back(i);
    }
    EXPECT_EQ(lst.size(), 100u);

    lst.clear();
    EXPECT_TRUE(lst.empty());

    for (int i = 0; i < 100; ++i) {
        lst.push_back(i * 2);
    }
    EXPECT_EQ(lst.size(), 100u);

    int expected = 0;
    for (const auto& item : lst) {
        EXPECT_EQ(item, expected);
        expected += 2;
    }
}
