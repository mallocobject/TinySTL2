#include "../ts_vector.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

using namespace TS;

void test_constructors()
{
    // 默认构造函数
    vector<int> v1;
    assert(v1.size() == 0);
    assert(v1.capacity() == VECTOR_INIT_SIZE);

    // 数量构造函数
    vector<int> v2(5);
    assert(v2.size() == 5);
    for (auto &x : v2)
        assert(x == 0);

    // 数量和值构造函数
    vector<int> v3(5, 42);
    assert(v3.size() == 5);
    for (auto &x : v3)
        assert(x == 42);

    // 拷贝构造函数
    vector<int> v4(v3);
    assert(v4.size() == 5);
    for (auto &x : v4)
        assert(x == 42);

    // 移动构造函数
    vector<int> v5(std::move(v4));
    assert(v5.size() == 5);
    for (auto &x : v5)
        assert(x == 42);
    assert(v4.size() == 0); // 被移动后应为空

    // 初始化列表构造函数
    vector<int> v6{1, 2, 3, 4, 5};
    assert(v6.size() == 5);
    for (int i = 0; i < 5; ++i)
        assert(v6[i] == i + 1);

    std::cout << "All constructor tests passed!\n";
}

void test_assignments()
{
    vector<int> v1{1, 2, 3};
    vector<int> v2;

    // 拷贝赋值
    v2 = v1;
    assert(v2.size() == 3);
    for (int i = 0; i < 3; ++i)
        assert(v2[i] == i + 1);

    // 移动赋值
    vector<int> v3;
    v3 = std::move(v2);
    assert(v3.size() == 3);
    for (int i = 0; i < 3; ++i)
        assert(v3[i] == i + 1);
    assert(v2.size() == 0); // 被移动后应为空

    // assign 方法
    vector<int> v4;
    v4.assign(v3);
    assert(v4.size() == 3);
    for (int i = 0; i < 3; ++i)
        assert(v4[i] == i + 1);

    std::cout << "All assignment tests passed!\n";
}

void test_element_access()
{
    vector<int> v{1, 2, 3};

    // operator[]
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);

    // at
    assert(v.at(0) == 1);
    assert(v.at(1) == 2);
    assert(v.at(2) == 3);
    try
    {
        v.at(3);
        assert(false); // 应该抛出异常
    }
    catch (const std::out_of_range &)
    {
    }

    // front 和 back
    assert(v.front() == 1);
    assert(v.back() == 3);

    // data
    int *p = v.data();
    assert(p[0] == 1);
    assert(p[1] == 2);
    assert(p[2] == 3);

    std::cout << "All element access tests passed!\n";
}

void test_iterators()
{
    vector<int> v{1, 2, 3};
    const vector<int> cv{1, 2, 3};

    // 非 const 迭代器
    int i = 1;
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        assert(*it == i++);
    }

    // const 迭代器
    i = 1;
    for (auto it = v.cbegin(); it != v.cend(); ++it)
    {
        assert(*it == i++);
    }

    // 范围 for 循环
    i = 1;
    for (const auto &x : v)
    {
        assert(x == i++);
    }

    std::cout << "All iterator tests passed!\n";
}

void test_capacity()
{
    vector<int> v;
    assert(v.empty());
    assert(v.size() == 0);

    v.push_back(1);
    assert(!v.empty());
    assert(v.size() == 1);

    // 容量测试
    assert(v.capacity() >= 1);
    size_t old_cap = v.capacity();
    v.reserve(old_cap + 10);
    assert(v.capacity() >= old_cap + 10);

    // shrink_to_fit
    v.shrink_to_fit();
    assert(v.capacity() == v.size());

    // max_size
    assert(v.max_size() > 0);

    std::cout << "All capacity tests passed!\n";
}

void test_modifiers()
{
    vector<int> v;

    // push_back
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    assert(v.size() == 3);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);

    // emplace_back
    v.emplace_back(4);
    assert(v.size() == 4);
    assert(v[3] == 4);

    // pop_back
    v.pop_back();
    assert(v.size() == 3);
    assert(v.back() == 3);

    // insert
    v.insert(v.begin() + 1, 99);
    assert(v.size() == 4);
    assert(v[0] == 1);
    assert(v[1] == 99);
    assert(v[2] == 2);
    assert(v[3] == 3);

    // emplace
    v.emplace(v.begin(), 0);
    assert(v.size() == 5);
    assert(v[0] == 0);
    assert(v[1] == 1);

    // erase
    v.erase(v.begin() + 1);
    assert(v.size() == 4);
    assert(v[0] == 0);
    assert(v[1] == 99);

    // erase range
    v.erase(v.begin() + 1, v.begin() + 3);
    assert(v.size() == 2);
    assert(v[0] == 0);
    assert(v[1] == 3);

    // clear
    v.clear();
    assert(v.size() == 0);
    assert(v.empty());

    // resize
    v.resize(5);
    assert(v.size() == 5);
    for (auto &x : v)
        assert(x == 0);

    v.resize(3, 42);
    assert(v.size() == 3);
    for (auto &x : v)
        assert(x == 0); // 缩小尺寸，值不变

    v.resize(5, 42);
    assert(v.size() == 5);
    for (int i = 0; i < 3; ++i)
        assert(v[i] == 0);
    for (int i = 3; i < 5; ++i)
        assert(v[i] == 42);

    // swap
    vector<int> v1{1, 2, 3};
    vector<int> v2{4, 5};
    v1.swap(v2);
    assert(v1.size() == 2);
    assert(v2.size() == 3);
    assert(v1[0] == 4);
    assert(v2[0] == 1);

    std::cout << "All modifier tests passed!\n";
}

void test_exceptions()
{
    vector<int> v{1, 2, 3};

    // at 越界
    try
    {
        v.at(10);
        assert(false);
    }
    catch (const std::out_of_range &)
    {
    }

    // 空向量 front/back
    vector<int> empty;
    try
    {
        empty.front();
        assert(false);
    }
    catch (const std::logic_error &)
    {
    }
    try
    {
        empty.back();
        assert(false);
    }
    catch (const std::logic_error &)
    {
    }

    std::cout << "All exception tests passed!\n";
}

void test_strings()
{
    // 测试字符串类型
    vector<std::string> v;
    v.push_back("hello");
    v.emplace_back("world");
    assert(v.size() == 2);
    assert(v[0] == "hello");
    assert(v[1] == "world");

    v.insert(v.begin() + 1, "test");
    assert(v[1] == "test");

    v.pop_back();
    assert(v.back() == "test");

    std::cout << "All string tests passed!\n";
}

void test_algorithms()
{
    // 确保与标准算法兼容
    vector<int> v{5, 3, 1, 4, 2};
    std::sort(v.begin(), v.end());
    for (int i = 0; i < 5; ++i)
        assert(v[i] == i + 1);

    auto it = std::find(v.begin(), v.end(), 3);
    assert(it != v.end());
    assert(*it == 3);

    std::cout << "All algorithm tests passed!\n";
}

int main()
{
    test_constructors();
    test_assignments();
    test_element_access();
    test_iterators();
    test_capacity();
    test_modifiers();
    test_exceptions();
    test_strings();
    test_algorithms();

    std::cout << "\nAll tests passed! Vector implementation is correct.\n";
    return 0;
}