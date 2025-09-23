#include "../ts_list.hpp"
#include <cassert>
#include <iostream>
#include <string>

// 测试类
struct TestStruct
{
    int id;
    std::string name;

    TestStruct() : id(0), name("default")
    {
        std::cout << "TestStruct default constructor: " << id << ", " << name << std::endl;
    }

    TestStruct(int i, const std::string &n) : id(i), name(n)
    {
        std::cout << "TestStruct constructor: " << id << ", " << name << std::endl;
    }

    TestStruct(const TestStruct &other) : id(other.id), name(other.name)
    {
        std::cout << "TestStruct copy constructor: " << id << ", " << name << std::endl;
    }

    TestStruct(TestStruct &&other) noexcept : id(other.id), name(std::move(other.name))
    {
        std::cout << "TestStruct move constructor: " << id << ", " << name << std::endl;
    }

    ~TestStruct()
    {
        std::cout << "TestStruct destructor: " << id << ", " << name << std::endl;
    }

    TestStruct &operator=(const TestStruct &other)
    {
        id = other.id;
        name = other.name;
        std::cout << "TestStruct copy assignment: " << id << ", " << name << std::endl;
        return *this;
    }

    TestStruct &operator=(TestStruct &&other) noexcept
    {
        id = other.id;
        name = std::move(other.name);
        std::cout << "TestStruct move assignment: " << id << ", " << name << std::endl;
        return *this;
    }

    bool operator==(const TestStruct &other) const
    {
        return id == other.id && name == other.name;
    }
};

void test_constructor()
{
    std::cout << "=== Testing constructors ===" << std::endl;

    // 默认构造函数
    TS::list<int> list1;
    std::cout << "Default constructor: OK" << std::endl;

    // // 初始化列表构造函数
    // TS::list<int> list2{1, 2, 3, 4, 5};
    // std::cout << "Initializer list constructor: OK" << std::endl;
}

void test_insert()
{
    std::cout << "\n=== Testing insert ===" << std::endl;

    TS::list<int> list;

    // 插入到空列表
    auto it = list.insert(list.end(), 1);
    std::cout << "Insert into empty list: OK" << std::endl;

    // 插入到开头
    list.insert(list.begin(), 0);
    std::cout << "Insert at beginning: OK" << std::endl;

    // 插入到中间
    list.insert(++list.begin(), 2);
    std::cout << "Insert in middle: OK" << std::endl;

    // 插入多个元素
    list.insert(list.end(), 3, 5);
    std::cout << "Insert multiple elements: OK" << std::endl;
}

void test_emplace()
{
    std::cout << "\n=== Testing emplace ===" << std::endl;

    TS::list<TestStruct> list;

    // 测试emplace
    list.emplace(list.end(), 1, "first");
    list.emplace(list.end(), 2, "second");
    list.emplace(list.begin(), 0, "zero");

    std::cout << "Emplace test: OK" << std::endl;
}

void test_copy_move()
{
    std::cout << "\n=== Testing copy and move ===" << std::endl;

    // TS::list<int> original{1, 2, 3, 4, 5};

    // // 拷贝构造
    // TS::list<int> copy = original;
    // std::cout << "Copy constructor: OK" << std::endl;

    // // 移动构造
    // TS::list<int> moved = std::move(original);
    // std::cout << "Move constructor: OK" << std::endl;
}

void test_clear()
{
    std::cout << "\n=== Testing clear ===" << std::endl;

    TS::list<TestStruct> list;
    list.emplace(list.end(), 1, "one");
    list.emplace(list.end(), 2, "two");
    list.emplace(list.end(), 3, "three");

    std::cout << "Before clear - list contains " << "some elements" << std::endl;
    list.clear();
    std::cout << "After clear - list should be empty" << std::endl;
}

// void test_iterator()
// {
//     std::cout << "\n=== Testing iterators ===" << std::endl;

//     TS::list<int> list{1, 2, 3, 4, 5};

//     // 前向迭代
//     std::cout << "Forward iteration: ";
//     for (auto it = list.begin(); it != list.end(); ++it)
//     {
//         std::cout << *it << " ";
//     }
//     std::cout << std::endl;

//     // 反向迭代（需要实现反向迭代器或使用--操作）
//     std::cout << "Backward iteration: ";
//     auto it = list.end();
//     --it; // 移动到最后一个元素
//     while (true)
//     {
//         std::cout << *it << " ";
//         if (it == list.begin())
//             break;
//         --it;
//     }
//     std::cout << std::endl;
// }

// void test_complex_types()
// {
//     std::cout << "\n=== Testing complex types ===" << std::endl;

//     TS::list<std::string> string_list;
//     string_list.insert(string_list.end(), "hello");
//     string_list.insert(string_list.end(), "world");
//     string_list.insert(string_list.begin(), "test");

//     std::cout << "String list: ";
//     for (auto it = string_list.begin(); it != string_list.end(); ++it)
//     {
//         std::cout << *it << " ";
//     }
//     std::cout << std::endl;
// }

void test_edge_cases()
{
    std::cout << "\n=== Testing edge cases ===" << std::endl;

    // 空列表操作
    TS::list<int> empty_list;
    empty_list.insert(empty_list.end(), 42);
    std::cout << "Insert into empty list: OK" << std::endl;

    // // 单个元素列表
    // TS::list<int> single_list(1, 100);
    // single_list.insert(single_list.begin(), 99);
    // single_list.insert(single_list.end(), 101);
    // std::cout << "Single element list operations: OK" << std::endl;
}

void test_performance()
{
    std::cout << "\n=== Testing performance ===" << std::endl;

    TS::list<int> list;

    // 大量插入测试
    for (int i = 0; i < 1000; ++i)
    {
        list.insert(list.end(), i);
    }
    std::cout << "Bulk insert test: OK" << std::endl;

    // 大量emplace测试
    TS::list<std::pair<int, std::string>> pair_list;
    for (int i = 0; i < 100; ++i)
    {
        pair_list.emplace(pair_list.end(), i, "test");
    }
    std::cout << "Bulk emplace test: OK" << std::endl;
}

int main()
{
    try
    {
        test_constructor();
        test_insert();
        test_emplace();
        test_copy_move();
        test_clear();
        // test_iterator();
        // test_complex_types();
        test_edge_cases();
        test_performance();

        std::cout << "\n=== All tests passed! ===" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}