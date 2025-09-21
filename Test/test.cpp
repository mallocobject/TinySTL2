#include "../ts_vector.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace TS;

// 测试辅助函数：打印测试结果
void print_test_result(const char *test_name, bool passed)
{
    std::cout << test_name << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
}

// 测试构造函数
void test_constructors()
{
    std::cout << "=== Testing constructors ===" << std::endl;

    // 默认构造函数
    vector<int> v1;
    bool passed = v1.size() == 0 && v1.capacity() == VECTOR_INIT_SIZE && v1.empty();
    print_test_result("Default constructor", passed);

    // 带大小的构造函数
    vector<int> v2(5);
    passed = v2.size() == 5 && v2.capacity() >= 5;
    for (size_t i = 0; i < v2.size(); ++i)
    {
        passed = passed && (v2[i] == 0);
    }
    print_test_result("Size constructor", passed);

    // 带大小和初始值的构造函数
    vector<int> v3(5, 42);
    passed = v3.size() == 5;
    for (size_t i = 0; i < v3.size(); ++i)
    {
        passed = passed && (v3[i] == 42);
    }
    print_test_result("Size and value constructor", passed);

    // 拷贝构造函数
    vector<int> v4(v3);
    passed = v4.size() == v3.size();
    for (size_t i = 0; i < v4.size(); ++i)
    {
        passed = passed && (v4[i] == v3[i]);
    }
    print_test_result("Copy constructor", passed);

    // 移动构造函数
    vector<int> v5(std::move(v4));
    passed = v5.size() == 5 && v4.size() == 0;
    for (size_t i = 0; i < v5.size(); ++i)
    {
        passed = passed && (v5[i] == 42);
    }
    print_test_result("Move constructor", passed);

    // 初始化列表构造函数
    vector<int> v6{1, 2, 3, 4, 5};
    passed = v6.size() == 5;
    for (size_t i = 0; i < v6.size(); ++i)
    {
        passed = passed && (v6[i] == static_cast<int>(i + 1));
    }
    print_test_result("Initializer list constructor", passed);
}

// 测试赋值操作
void test_assignment()
{
    std::cout << "=== Testing assignment operations ===" << std::endl;

    vector<int> v1{1, 2, 3};
    vector<int> v2;

    // 拷贝赋值
    v2 = v1;
    bool passed = v2.size() == v1.size();
    for (size_t i = 0; i < v2.size(); ++i)
    {
        passed = passed && (v2[i] == v1[i]);
    }
    print_test_result("Copy assignment", passed);

    // 移动赋值
    vector<int> v3;
    v3 = std::move(v1);
    passed = v3.size() == 3 && v1.size() == 0;
    passed = passed && (v3[0] == 1 && v3[1] == 2 && v3[2] == 3);
    print_test_result("Move assignment", passed);

    // 自赋值
    v3 = v3;
    passed = v3.size() == 3 && (v3[0] == 1 && v3[1] == 2 && v3[2] == 3);
    print_test_result("Self assignment", passed);

    // assign方法
    vector<int> v4;
    v4.assign(v3);
    passed = v4.size() == v3.size();
    for (size_t i = 0; i < v4.size(); ++i)
    {
        passed = passed && (v4[i] == v3[i]);
    }
    print_test_result("Assign method", passed);
}

// 测试元素访问
void test_element_access()
{
    std::cout << "=== Testing element access ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};
    bool passed = true;

    // operator[]
    for (size_t i = 0; i < v.size(); ++i)
    {
        passed = passed && (v[i] == static_cast<int>(i + 1));
    }
    print_test_result("Operator[]", passed);

    // at() - 有效索引
    passed = true;
    for (size_t i = 0; i < v.size(); ++i)
    {
        passed = passed && (v.at(i) == static_cast<int>(i + 1));
    }
    print_test_result("At() with valid indices", passed);

    // at() - 无效索引
    passed = false;
    try
    {
        v.at(10);
    }
    catch (const std::out_of_range &e)
    {
        passed = true;
    }
    print_test_result("At() with invalid index", passed);

    // front和back
    passed = (v.front() == 1) && (v.back() == 5);
    print_test_result("Front and back", passed);

    // data
    int *data = v.data();
    passed = true;
    for (size_t i = 0; i < v.size(); ++i)
    {
        passed = passed && (data[i] == v[i]);
    }
    print_test_result("Data", passed);

    // 空vector的front/back测试
    vector<int> empty_v;
    passed = false;
    try
    {
        empty_v.front();
    }
    catch (const std::logic_error &e)
    {
        passed = true;
    }
    print_test_result("Front on empty vector", passed);

    passed = false;
    try
    {
        empty_v.back();
    }
    catch (const std::logic_error &e)
    {
        passed = true;
    }
    print_test_result("Back on empty vector", passed);
}

// 测试迭代器
void test_iterators()
{
    std::cout << "=== Testing iterators ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};
    bool passed = true;

    // 普通迭代器
    int expected = 1;
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        passed = passed && (*it == expected++);
    }
    print_test_result("Iterator traversal", passed);

    // const迭代器
    passed = true;
    expected = 1;
    for (auto it = v.cbegin(); it != v.cend(); ++it)
    {
        passed = passed && (*it == expected++);
    }
    print_test_result("Const iterator traversal", passed);

    // 基于范围的for循环
    passed = true;
    expected = 1;
    for (int val : v)
    {
        passed = passed && (val == expected++);
    }
    print_test_result("Range-based for loop", passed);

    // 空vector的迭代器
    vector<int> empty_v;
    passed = (empty_v.begin() == empty_v.end()) && (empty_v.cbegin() == empty_v.cend());
    print_test_result("Iterators on empty vector", passed);
}

// 测试容量操作
void test_capacity_operations()
{
    std::cout << "=== Testing capacity operations ===" << std::endl;

    vector<int> v;
    bool passed = v.empty() && v.size() == 0 && v.capacity() == VECTOR_INIT_SIZE;
    print_test_result("Empty vector", passed);

    // 添加元素
    for (int i = 0; i < 5; ++i)
    {
        v.push_back(i);
    }

    passed = !v.empty() && v.size() == 5 && v.capacity() >= 5;
    print_test_result("After adding elements", passed);

    // reserve - 扩大容量
    size_t old_capacity = v.capacity();
    v.reserve(old_capacity * 2);
    passed = v.capacity() >= old_capacity * 2 && v.size() == 5;
    for (int i = 0; i < 5; ++i)
    {
        passed = passed && (v[i] == i);
    }
    print_test_result("Reserve to increase capacity", passed);

    // reserve - 小于当前容量，应无变化
    old_capacity = v.capacity();
    v.reserve(old_capacity / 2);
    passed = v.capacity() == old_capacity;
    print_test_result("Reserve with smaller capacity", passed);

    // shrink_to_fit
    v.shrink_to_fit();
    passed = v.capacity() == v.size();
    for (int i = 0; i < 5; ++i)
    {
        passed = passed && (v[i] == i);
    }
    print_test_result("Shrink to fit", passed);

    // max_size
    passed = v.max_size() > 0;
    print_test_result("Max size", passed);
}

// 测试修改操作
void test_modifiers()
{
    std::cout << "=== Testing modifiers ===" << std::endl;

    vector<int> v;
    bool passed = true;

    // push_back - 左值
    for (int i = 0; i < 5; ++i)
    {
        v.push_back(i);
    }
    passed = v.size() == 5;
    for (int i = 0; i < 5; ++i)
    {
        passed = passed && (v[i] == i);
    }
    print_test_result("Push back lvalue", passed);

    // push_back - 右值
    v.push_back(5);
    passed = v.size() == 6 && v[5] == 5;
    print_test_result("Push back rvalue", passed);

    // emplace_back
    v.emplace_back(6);
    passed = v.size() == 7 && v[6] == 6;
    print_test_result("Emplace back", passed);

    // pop_back
    v.pop_back();
    passed = v.size() == 6;
    for (int i = 0; i < 6; ++i)
    {
        passed = passed && (v[i] == i);
    }
    print_test_result("Pop back", passed);

    // clear
    v.clear();
    passed = v.empty() && v.size() == 0 && v.capacity() > 0;
    print_test_result("Clear", passed);
}

// 测试插入和删除
void test_insert_erase()
{
    std::cout << "=== Testing insert and erase ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};
    bool passed = true;

    // 在开头插入
    v.insert(v.begin(), 0);
    passed = v.size() == 6 && v[0] == 0;
    for (int i = 1; i < 6; ++i)
    {
        passed = passed && (v[i] == i);
    }
    print_test_result("Insert at beginning", passed);

    // 在中间插入
    v.insert(v.begin() + 3, 99);
    passed = v.size() == 7 && v[3] == 99;
    print_test_result("Insert in middle", passed);

    // 在末尾插入
    v.insert(v.end(), 100);
    passed = v.size() == 8 && v[7] == 100;
    print_test_result("Insert at end", passed);

    // emplace
    v.emplace(v.begin() + 2, 88);
    passed = v.size() == 9 && v[2] == 88;
    print_test_result("Emplace", passed);

    // 删除单个元素
    v.erase(v.begin() + 2);
    passed = v.size() == 8 && v[2] != 88;
    print_test_result("Erase single element", passed);

    // 删除范围
    v.erase(v.begin() + 3, v.begin() + 5);
    passed = v.size() == 6;
    print_test_result("Erase range", passed);

    // 删除所有元素
    v.erase(v.begin(), v.end());
    passed = v.empty();
    print_test_result("Erase all elements", passed);
}

// 测试resize
void test_resize()
{
    std::cout << "=== Testing resize ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};
    bool passed = true;

    // 缩小尺寸
    v.resize(3);
    passed = v.size() == 3;
    for (int i = 0; i < 3; ++i)
    {
        passed = passed && (v[i] == i + 1);
    }
    print_test_result("Resize to smaller size", passed);

    // 扩大尺寸 - 默认值
    v.resize(5);
    passed = v.size() == 5;
    for (int i = 0; i < 3; ++i)
    {
        passed = passed && (v[i] == i + 1);
    }
    for (int i = 3; i < 5; ++i)
    {
        passed = passed && (v[i] == 0);
    }
    print_test_result("Resize to larger size with default value", passed);

    // 扩大尺寸 - 指定值
    v.resize(7, 99);
    passed = v.size() == 7;
    for (int i = 5; i < 7; ++i)
    {
        passed = passed && (v[i] == 99);
    }
    print_test_result("Resize to larger size with specified value", passed);
}

// 测试swap
void test_swap()
{
    std::cout << "=== Testing swap ===" << std::endl;

    vector<int> v1{1, 2, 3};
    vector<int> v2{4, 5, 6, 7};

    size_t v1_size = v1.size();
    size_t v2_size = v2.size();
    size_t v1_capacity = v1.capacity();
    size_t v2_capacity = v2.capacity();

    v1.swap(v2);

    bool passed = v1.size() == v2_size && v2.size() == v1_size;
    passed = passed && v1.capacity() == v2_capacity && v2.capacity() == v1_capacity;

    for (int i = 0; i < 4; ++i)
    {
        passed = passed && (v1[i] == i + 4);
    }
    for (int i = 0; i < 3; ++i)
    {
        passed = passed && (v2[i] == i + 1);
    }
    print_test_result("Swap", passed);
}

// 测试自定义类型
void test_custom_type()
{
    std::cout << "=== Testing custom type ===" << std::endl;

    vector<std::string> v;
    bool passed = true;

    // push_back
    v.push_back("Hello");
    v.push_back("World");
    passed = v.size() == 2 && v[0] == "Hello" && v[1] == "World";
    print_test_result("Push back with strings", passed);

    // emplace_back
    v.emplace_back("Test");
    passed = v.size() == 3 && v[2] == "Test";
    print_test_result("Emplace back with strings", passed);

    // insert
    v.insert(v.begin() + 1, "Middle");
    passed = v.size() == 4 && v[1] == "Middle";
    print_test_result("Insert with strings", passed);

    // erase
    v.erase(v.begin() + 2);
    passed = v.size() == 3 && v[0] == "Hello" && v[1] == "Middle" && v[2] == "Test";
    print_test_result("Erase with strings", passed);

    // resize
    v.resize(5, "Default");
    passed = v.size() == 5 && v[3] == "Default" && v[4] == "Default";
    print_test_result("Resize with strings", passed);
}

// 测试异常安全
void test_exception_safety()
{
    std::cout << "=== Testing exception safety ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};
    bool passed = true;

    // at() 超出范围应抛出异常
    try
    {
        v.at(10);
        passed = false; // 不应该执行到这里
    }
    catch (const std::out_of_range &)
    {
        // 预期行为
    }
    print_test_result("Out of range exception", passed);

    // 空vector的front/back应抛出异常
    vector<int> empty_v;
    try
    {
        empty_v.front();
        passed = false; // 不应该执行到这里
    }
    catch (const std::logic_error &)
    {
        // 预期行为
    }
    print_test_result("Front on empty vector exception", passed);

    try
    {
        empty_v.back();
        passed = false; // 不应该执行到这里
    }
    catch (const std::logic_error &)
    {
        // 预期行为
    }
    print_test_result("Back on empty vector exception", passed);
}

// 测试与STL算法的兼容性
void test_stl_compatibility()
{
    std::cout << "=== Testing STL compatibility ===" << std::endl;

    vector<int> v{5, 3, 1, 4, 2};
    bool passed = true;

    // 使用STL排序
    std::sort(v.begin(), v.end());
    for (int i = 0; i < 5; ++i)
    {
        passed = passed && (v[i] == i + 1);
    }
    print_test_result("STL sort", passed);

    // 使用STL查找
    auto it = std::find(v.begin(), v.end(), 3);
    passed = (it != v.end()) && (*it == 3);
    print_test_result("STL find", passed);

    // 使用STL填充
    std::fill(v.begin(), v.end(), 42);
    for (int val : v)
    {
        passed = passed && (val == 42);
    }
    print_test_result("STL fill", passed);
}

// 测试相等操作符
void test_equality_operator()
{
    std::cout << "=== Testing equality operator ===" << std::endl;

    vector<int> v1{1, 2, 3, 4, 5};
    vector<int> v2{1, 2, 3, 4, 5};
    vector<int> v3{1, 2, 3};
    vector<int> v4{1, 2, 3, 4, 6};

    bool passed = (v1 == v2) && !(v1 == v3) && !(v1 == v4);
    print_test_result("Equality operator", passed);

    // 测试空向量
    vector<int> v5;
    vector<int> v6;
    passed = (v5 == v6);
    print_test_result("Equality operator with empty vectors", passed);

    // 测试自定义类型
    vector<std::string> s1{"hello", "world"};
    vector<std::string> s2{"hello", "world"};
    vector<std::string> s3{"hello", "world!"};
    passed = (s1 == s2) && !(s1 == s3);
    print_test_result("Equality operator with strings", passed);
}

// 性能测试 - 大量数据
void test_performance()
{
    std::cout << "=== Testing performance ===" << std::endl;

    const int N = 10000;
    vector<int> v;
    bool passed = true;

    // 测试push_back性能
    for (int i = 0; i < N; ++i)
    {
        v.push_back(i);
    }
    passed = v.size() == N;
    for (int i = 0; i < N; ++i)
    {
        passed = passed && (v[i] == i);
    }
    print_test_result("Performance - push_back", passed);

    // 测试reserve
    v.reserve(N * 2);
    passed = v.capacity() >= N * 2 && v.size() == N;
    for (int i = 0; i < N; ++i)
    {
        passed = passed && (v[i] == i);
    }
    print_test_result("Performance - reserve", passed);

    // 测试clear
    v.clear();
    passed = v.empty() && v.capacity() >= N * 2;
    print_test_result("Performance - clear", passed);
}

int main()
{
    try
    {
        test_constructors();
        test_assignment();
        test_element_access();
        test_iterators();
        test_capacity_operations();
        test_modifiers();
        test_insert_erase();
        test_resize();
        test_swap();
        test_custom_type();
        test_exception_safety();
        test_stl_compatibility();
        test_equality_operator();
        test_performance();

        std::cout << "\nAll tests completed!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Test failed with unknown exception." << std::endl;
        return 1;
    }
}