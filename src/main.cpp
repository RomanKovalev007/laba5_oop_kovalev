#include <iostream>
#include <memory_resource>
#include <cstdint>
#include <new>
#include <string>
#include "../include/my_memory_resource.h"
#include "../include/linked_list.h"

struct Person {
    int id;
    pmr::string name; 
    Person(int i, pmr::string n) : id(i), name(std::move(n)) {}
};

int main() {
    try {
        constexpr std::size_t BUF = 1 << 20;
        MyMemoryResource myres(BUF);

        {
            std::cout << "int list:\n\n";
            pmr::polymorphic_allocator<int> alloc_int{&myres};
            LinkedList<int> ilist{&myres};
            ilist.push_back(1);
            ilist.push_back(20);
            ilist.push_front(5);
            std::cout << "int list elements:";
            for (auto &v : ilist) std::cout << " " << v;
            std::cout << "\nsize = " << ilist.size() << "\n";

            ilist.pop_front();
            std::cout << "after pop_front:";
            for (auto &v : ilist) std::cout << " " << v;
            std::cout << "\nsize = " << ilist.size() << "\n\n";
        }

        {
            std::cout << "person list:\n\n";
            pmr::string n1{&myres}; n1 = "Roma";
            pmr::string n2{&myres}; n2 = "Tolya";
            pmr::string n3{&myres}; n3 = "Andrew";

            LinkedList<Person> plist{&myres};
            plist.push_back(Person{1, std::move(n1)});
            plist.push_back(Person{2, std::move(n2)});
            plist.push_front(Person{0, std::move(n3)});

            std::cout << "person list elements:\n";
            for (auto &p : plist) {
                std::cout << "  Person{id=" << p.id << ", name=\"" << p.name << "\"}\n";
            }
            std::cout << "size = " << plist.size() << "\n\n";

            plist.clear();
            std::cout << "after clear, size = " << plist.size() << "\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "exception: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
