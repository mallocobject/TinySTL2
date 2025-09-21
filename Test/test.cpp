#include "../ts_vector.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace TS;

// 测试基础功能
void test_basic_functionality()
{
    std::cout << "=== Testing basic functionality ===" << std::endl;

    // 默认构造函数
    vector<int> v1;
    assert(v1.size() == 0);
    assert(v1.capacity() == VECTOR_INIT_SIZE);
    assert(v1.empty());

    // 带大小的构造函数
    vector<int> v2(5);
    assert(v2.size() == 5);
    assert(v2.capacity() >= 5);
    for (size_t i = 0; i < v2.size(); ++i)
    {
        assert(v2[i] == 0);
    }

    // 带大小和初始值的构造函数
    vector<int> v3(5, 42);
    assert(v3.size() == 5);
    for (size_t i = 0; i < v3.size(); ++i)
    {
        assert(v3[i] == 42);
    }

    // 拷贝构造函数
    vector<int> v4(v3);
    assert(v4.size() == v3.size());
    for (size_t i = 0; i < v4.size(); ++i)
    {
        assert(v4[i] == v3[i]);
    }

    // 移动构造函数
    vector<int> v5(std::move(v4));
    assert(v5.size() == 5);
    assert(v4.size() == 0); // 移动后原对象应为空
    for (size_t i = 0; i < v5.size(); ++i)
    {
        assert(v5[i] == 42);
    }

    // 初始化列表构造函数
    vector<int> v6{1, 2, 3, 4, 5};
    assert(v6.size() == 5);
    for (size_t i = 0; i < v6.size(); ++i)
    {
        assert(v6[i] == static_cast<int>(i + 1));
    }

    std::cout << "Basic functionality tests passed!" << std::endl;
}

// 测试赋值操作
void test_assignment()
{
    std::cout << "=== Testing assignment operations ===" << std::endl;

    vector<int> v1{1, 2, 3};
    vector<int> v2;

    // 拷贝赋值
    v2 = v1;
    assert(v2.size() == v1.size());
    for (size_t i = 0; i < v2.size(); ++i)
    {
        assert(v2[i] == v1[i]);
    }

    // 移动赋值
    vector<int> v3;
    v3 = std::move(v1);
    assert(v3.size() == 3);
    assert(v1.size() == 0); // 移动后原对象应为空
    assert(v3[0] == 1 && v3[1] == 2 && v3[2] == 3);

    // 自赋值
    v3 = v3;
    assert(v3.size() == 3);
    assert(v3[0] == 1 && v3[1] == 2 && v3[2] == 3);

    // assign方法
    vector<int> v4;
    v4.assign(v3);
    assert(v4.size() == v3.size());
    for (size_t i = 0; i < v4.size(); ++i)
    {
        assert(v4[i] == v3[i]);
    }

    std::cout << "Assignment tests passed!" << std::endl;
}

// 测试元素访问
void test_element_access()
{
    std::cout << "=== Testing element access ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};

    // operator[]
    for (size_t i = 0; i < v.size(); ++i)
    {
        assert(v[i] == static_cast<int>(i + 1));
    }

    // at() - 有效索引
    for (size_t i = 0; i < v.size(); ++i)
    {
        assert(v.at(i) == static_cast<int>(i + 1));
    }

    // at() - 无效索引
    try
    {
        v.at(10);
        assert(false); // 应该抛出异常
    }
    catch (const std::out_of_range &e)
    {
        // 预期行为
    }

    // front和back
    assert(v.front() == 1);
    assert(v.back() == 5);

    // data
    int *data = v.data();
    for (size_t i = 0; i < v.size(); ++i)
    {
        assert(data[i] == v[i]);
    }

    // 空vector的front/back测试
    vector<int> empty_v;
    try
    {
        empty_v.front();
        assert(false); // 应该抛出异常
    }
    catch (const std::logic_error &e)
    {
        // 预期行为
    }

    try
    {
        empty_v.back();
        assert(false); // 应该抛出异常
    }
    catch (const std::logic_error &e)
    {
        // 预期行为
    }

    std::cout << "Element access tests passed!" << std::endl;
}

// 测试迭代器
void test_iterators()
{
    std::cout << "=== Testing iterators ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};

    // 普通迭代器
    int expected = 1;
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        assert(*it == expected++);
    }

    // const迭代器
    expected = 1;
    for (auto it = v.cbegin(); it != v.cend(); ++it)
    {
        assert(*it == expected++);
    }

    // 基于范围的for循环
    expected = 1;
    for (int val : v)
    {
        assert(val == expected++);
    }

    // 空vector的迭代器
    vector<int> empty_v;
    assert(empty_v.begin() == empty_v.end());
    assert(empty_v.cbegin() == empty_v.cend());

    std::cout << "Iterator tests passed!" << std::endl;
}

// 测试容量操作
void test_capacity_operations()
{
    std::cout << "=== Testing capacity operations ===" << std::endl;

    vector<int> v;
    assert(v.empty());
    assert(v.size() == 0);
    assert(v.capacity() == VECTOR_INIT_SIZE);

    // 添加元素
    for (int i = 0; i < 5; ++i)
    {
        v.push_back(i);
    }

    assert(!v.empty());
    assert(v.size() == 5);
    assert(v.capacity() >= 5);

    // reserve - 扩大容量
    size_t old_capacity = v.capacity();
    v.reserve(old_capacity * 2);
    assert(v.capacity() >= old_capacity * 2);
    assert(v.size() == 5);
    for (int i = 0; i < 5; ++i)
    {
        assert(v[i] == i);
    }

    // reserve - 小于当前容量，应无变化
    old_capacity = v.capacity();
    v.reserve(old_capacity / 2);
    assert(v.capacity() == old_capacity);

    // shrink_to_fit
    v.shrink_to_fit();
    assert(v.capacity() == v.size());
    for (int i = 0; i < 5; ++i)
    {
        assert(v[i] == i);
    }

    // max_size
    assert(v.max_size() > 0);

    std::cout << "Capacity operations tests passed!" << std::endl;
}

// 测试修改操作
void test_modifiers()
{
    std::cout << "=== Testing modifiers ===" << std::endl;

    vector<int> v;

    // push_back - 左值
    for (int i = 0; i < 5; ++i)
    {
        v.push_back(i);
    }
    assert(v.size() == 5);
    for (int i = 0; i < 5; ++i)
    {
        assert(v[i] == i);
    }

    // push_back - 右值
    v.push_back(5);
    assert(v.size() == 6);
    assert(v[5] == 5);

    // emplace_back
    v.emplace_back(6);
    assert(v.size() == 7);
    assert(v[6] == 6);

    // pop_back
    v.pop_back();
    assert(v.size() == 6);
    for (int i = 0; i < 6; ++i)
    {
        assert(v[i] == i);
    }

    // clear
    v.clear();
    assert(v.empty());
    assert(v.size() == 0);
    assert(v.capacity() > 0); // 清空后容量应保持不变

    std::cout << "Modifiers tests passed!" << std::endl;
}

// 测试插入和删除
void test_insert_erase()
{
    std::cout << "=== Testing insert and erase ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};

    // 在开头插入
    v.insert(v.begin(), 0);
    assert(v.size() == 6);
    assert(v[0] == 0);
    for (int i = 1; i < 6; ++i)
    {
        assert(v[i] == i);
    }

    // 在中间插入
    v.insert(v.begin() + 3, 99);
    assert(v.size() == 7);
    assert(v[3] == 99);

    // 在末尾插入
    v.insert(v.end(), 100);
    assert(v.size() == 8);
    assert(v[7] == 100);

    // emplace
    v.emplace(v.begin() + 2, 88);
    assert(v.size() == 9);
    assert(v[2] == 88);

    // 删除单个元素
    v.erase(v.begin() + 2);
    assert(v.size() == 8);
    assert(v[2] != 88);

    // 删除范围
    v.erase(v.begin() + 3, v.begin() + 5);
    assert(v.size() == 6);

    // 删除所有元素
    v.erase(v.begin(), v.end());
    assert(v.empty());

    std::cout << "Insert and erase tests passed!" << std::endl;
}

// 测试resize
void test_resize()
{
    std::cout << "=== Testing resize ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};

    // 缩小尺寸
    v.resize(3);
    assert(v.size() == 3);
    for (int i = 0; i < 3; ++i)
    {
        assert(v[i] == i + 1);
    }

    // 扩大尺寸 - 默认值
    v.resize(5);
    assert(v.size() == 5);
    for (int i = 0; i < 3; ++i)
    {
        assert(v[i] == i + 1);
    }
    for (int i = 3; i < 5; ++i)
    {
        assert(v[i] == 0);
    }

    // 扩大尺寸 - 指定值
    v.resize(7, 99);
    assert(v.size() == 7);
    for (int i = 5; i < 7; ++i)
    {
        assert(v[i] == 99);
    }

    std::cout << "Resize tests passed!" << std::endl;
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

    assert(v1.size() == v2_size);
    assert(v2.size() == v1_size);
    assert(v1.capacity() == v2_capacity);
    assert(v2.capacity() == v1_capacity);

    for (int i = 0; i < 4; ++i)
    {
        assert(v1[i] == i + 4);
    }
    for (int i = 0; i < 3; ++i)
    {
        assert(v2[i] == i + 1);
    }

    std::cout << "Swap tests passed!" << std::endl;
}

// 测试自定义类型
void test_custom_type()
{
    std::cout << "=== Testing custom type ===" << std::endl;

    vector<std::string> v;

    // push_back
    v.push_back("Hello");
    v.push_back("World");
    assert(v.size() == 2);
    assert(v[0] == "Hello");
    assert(v[1] == "World");

    // emplace_back
    v.emplace_back("Test");
    assert(v.size() == 3);
    assert(v[2] == "Test");

    // insert
    v.insert(v.begin() + 1, "Middle");
    assert(v.size() == 4);
    assert(v[1] == "Middle");

    // erase
    v.erase(v.begin() + 2);
    assert(v.size() == 3);
    assert(v[0] == "Hello");
    assert(v[1] == "Middle");
    assert(v[2] == "Test");

    // resize
    v.resize(5, "Default");
    assert(v.size() == 5);
    assert(v[3] == "Default");
    assert(v[4] == "Default");

    std::cout << "Custom type tests passed!" << std::endl;
}

// 测试异常安全
void test_exception_safety()
{
    std::cout << "=== Testing exception safety ===" << std::endl;

    vector<int> v{1, 2, 3, 4, 5};

    // at() 超出范围应抛出异常
    try
    {
        v.at(10);
        assert(false); // 不应该执行到这里
    }
    catch (const std::out_of_range &)
    {
        // 预期行为
    }

    // 空vector的front/back应抛出异常
    vector<int> empty_v;
    try
    {
        empty_v.front();
        assert(false); // 不应该执行到这里
    }
    catch (const std::logic_error &)
    {
        // 预期行为
    }

    try
    {
        empty_v.back();
        assert(false); // 不应该执行到这里
    }
    catch (const std::logic_error &)
    {
        // 预期行为
    }

    std::cout << "Exception safety tests passed!" << std::endl;
}

// 测试与STL算法的兼容性
void test_stl_compatibility()
{
    std::cout << "=== Testing STL compatibility ===" << std::endl;

    vector<int> v{5, 3, 1, 4, 2};

    // 使用STL排序
    std::sort(v.begin(), v.end());
    for (int i = 0; i < 5; ++i)
    {
        assert(v[i] == i + 1);
    }

    // 使用STL查找
    auto it = std::find(v.begin(), v.end(), 3);
    assert(it != v.end());
    assert(*it == 3);

    // 使用STL填充
    std::fill(v.begin(), v.end(), 42);
    for (int val : v)
    {
        assert(val == 42);
    }

    std::cout << "STL compatibility tests passed!" << std::endl;
}

// 性能测试 - 大量数据
void test_performance()
{
    std::cout << "=== Testing performance ===" << std::endl;

    const int N = 10000;
    vector<int> v;

    // 测试push_back性能
    for (int i = 0; i < N; ++i)
    {
        v.push_back(i);
    }
    assert(v.size() == N);
    for (int i = 0; i < N; ++i)
    {
        assert(v[i] == i);
    }

    // 测试reserve
    v.reserve(N * 2);
    assert(v.capacity() >= N * 2);
    assert(v.size() == N);
    for (int i = 0; i < N; ++i)
    {
        assert(v[i] == i);
    }

    // 测试clear
    v.clear();
    assert(v.empty());
    assert(v.capacity() >= N * 2); // 容量应保持不变

    std::cout << "Performance tests passed!" << std::endl;
}

int main()
{
    try
    {
        test_basic_functionality();
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
        test_performance();

        std::cout << "\nAll tests passed! Vector implementation is correct." << std::endl;
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