#include "ts_list.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace TS_Test
{

void test_constructors_and_destructor()
{
    std::cout << "=== Testing constructors and destructor ===" << std::endl;

    // 默认构造函数
    {
        TS::list<int> list;
        assert(list.empty());
        assert(list.size() == 0);
        assert(list.begin() == list.end());
    }

    // 大小构造函数
    {
        TS::list<int> list(5);
        assert(list.size() == 5);
    }

    // 大小和值构造函数
    {
        TS::list<int> list(3, 42);
        assert(list.size() == 3);
        for (int value : list)
        {
            assert(value == 42);
        }
    }

    // 拷贝构造函数
    {
        TS::list<int> original{1, 2, 3};
        TS::list<int> copy(original);
        assert(original.size() == copy.size());
        auto it1 = original.begin();
        auto it2 = copy.begin();
        while (it1 != original.end())
        {
            assert(*it1 == *it2);
            ++it1;
            ++it2;
        }
    }

    // 移动构造函数
    {
        TS::list<int> original{1, 2, 3};
        TS::list<int> moved(std::move(original));
        assert(moved.size() == 3);
        assert(original.empty());
    }

    // 初始化列表构造函数
    {
        TS::list<int> list{1, 2, 3, 4, 5};
        assert(list.size() == 5);
        int expected = 1;
        for (int value : list)
        {
            assert(value == expected++);
        }
    }

    std::cout << "All constructor tests passed!" << std::endl;
}

void test_assignment_operators()
{
    std::cout << "=== Testing assignment operators ===" << std::endl;

    // 拷贝赋值
    {
        TS::list<int> list1{1, 2, 3};
        TS::list<int> list2;
        list2 = list1;
        assert(list1.size() == list2.size());
        auto it1 = list1.begin();
        auto it2 = list2.begin();
        while (it1 != list1.end())
        {
            assert(*it1 == *it2);
            ++it1;
            ++it2;
        }
    }

    // 自赋值
    {
        TS::list<int> list{1, 2, 3};
        list = list;
        assert(list.size() == 3);
    }

    // 移动赋值
    {
        TS::list<int> list1{1, 2, 3};
        TS::list<int> list2;
        list2 = std::move(list1);
        assert(list2.size() == 3);
        assert(list1.empty());
    }

    // 初始化列表赋值
    {
        TS::list<int> list;
        list = {10, 20, 30, 40};
        assert(list.size() == 4);
        assert(list.front() == 10);
        assert(list.back() == 40);
    }

    std::cout << "All assignment tests passed!" << std::endl;
}

void test_element_access()
{
    std::cout << "=== Testing element access ===" << std::endl;

    TS::list<int> list{10, 20, 30};

    // front 和 back
    assert(list.front() == 10);
    assert(list.back() == 30);

    // 修改元素
    list.front() = 100;
    list.back() = 300;
    assert(list.front() == 100);
    assert(list.back() == 300);

    // 空列表访问测试
    TS::list<int> empty_list;
    bool front_exception_thrown = false;
    bool back_exception_thrown = false;

    try
    {
        empty_list.front();
    }
    catch (const std::range_error &)
    {
        front_exception_thrown = true;
    }

    try
    {
        empty_list.back();
    }
    catch (const std::range_error &)
    {
        back_exception_thrown = true;
    }

    assert(front_exception_thrown);
    assert(back_exception_thrown);

    std::cout << "All element access tests passed!" << std::endl;
}

void test_iterators()
{
    std::cout << "=== Testing iterators ===" << std::endl;

    TS::list<int> list{1, 2, 3, 4, 5};

    // 正向迭代
    int sum = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        sum += *it;
    }
    assert(sum == 15);

    // 范围for循环
    sum = 0;
    for (int value : list)
    {
        sum += value;
    }
    assert(sum == 15);

    // const 迭代器
    const TS::list<int> &const_list = list;
    sum = 0;
    for (auto it = const_list.begin(); it != const_list.end(); ++it)
    {
        sum += *it;
    }
    assert(sum == 15);

    // cbegin/cend
    sum = 0;
    for (auto it = list.cbegin(); it != list.cend(); ++it)
    {
        sum += *it;
    }
    assert(sum == 15);

    // 迭代器算术运算
    auto it = list.begin();
    assert(*it == 1);
    ++it;
    assert(*it == 2);
    --it;
    assert(*it == 1);

    // 后置递增
    it = list.begin();
    auto old_it = it++;
    assert(*old_it == 1);
    assert(*it == 2);

    // 后置递减
    it = ++list.begin();
    old_it = it--;
    assert(*old_it == 2);
    assert(*it == 1);

    // 迭代器比较
    auto it1 = list.begin();
    auto it2 = list.begin();
    assert(it1 == it2);
    ++it2;
    assert(it1 != it2);

    std::cout << "All iterator tests passed!" << std::endl;
}

void test_capacity()
{
    std::cout << "=== Testing capacity methods ===" << std::endl;

    TS::list<int> list;

    // empty 和 size
    assert(list.empty());
    assert(list.size() == 0);

    list = {1, 2, 3};
    assert(!list.empty());
    assert(list.size() == 3);

    // max_size
    assert(list.max_size() > 0);

    // clear
    list.clear();
    assert(list.empty());
    assert(list.size() == 0);

    // clear 空列表
    TS::list<int> empty_list;
    empty_list.clear();
    assert(empty_list.empty());

    std::cout << "All capacity tests passed!" << std::endl;
}

void test_modifiers()
{
    std::cout << "=== Testing modifiers ===" << std::endl;

    TS::list<int> list;

    // insert 单个元素（左值）
    auto it = list.insert(list.end(), 10);
    assert(list.size() == 1);
    assert(*it == 10);
    assert(list.front() == 10);
    assert(list.back() == 10);

    // insert 单个元素（右值）
    it = list.insert(list.begin(), 5);
    assert(list.size() == 2);
    assert(*it == 5);
    assert(list.front() == 5);
    assert(list.back() == 10);

    // insert 多个相同元素
    it = list.insert(list.end(), 3, 20);
    assert(list.size() == 5);
    assert(*it == 20);

    // insert 初始化列表
    it = list.insert(list.begin(), {1, 2, 3});
    assert(list.size() == 8);
    assert(*it == 1);
    assert(list.front() == 1);

    // emplace
    it = list.emplace(list.end(), 100);
    assert(list.size() == 9);
    assert(*it == 100);
    assert(list.back() == 100);

    // erase 单个元素
    it = list.erase(list.begin());
    assert(list.size() == 8);
    assert(*it == 2);

    // erase 范围
    auto first = list.begin();
    auto last = list.begin();
    ++last;
    ++last;
    it = list.erase(first, last);
    assert(list.size() == 6);

    // erase 所有元素
    list.erase(list.begin(), list.end());
    assert(list.empty());

    std::cout << "All modifier tests passed!" << std::endl;
}

void test_push_pop_operations()
{
    std::cout << "=== Testing push/pop operations ===" << std::endl;

    TS::list<int> list;

    // push_back
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    assert(list.size() == 3);
    assert(list.front() == 1);
    assert(list.back() == 3);

    // push_back 右值
    list.push_back(std::move(4));
    assert(list.back() == 4);

    // push_front
    list.push_front(0);
    assert(list.size() == 5);
    assert(list.front() == 0);

    // push_front 右值
    list.push_front(std::move(-1));
    assert(list.front() == -1);

    // emplace_back
    list.emplace_back(5);
    assert(list.back() == 5);

    // emplace_front
    list.emplace_front(-2);
    assert(list.front() == -2);

    // pop_back
    list.pop_back();
    assert(list.back() == 4);

    // pop_front
    list.pop_front();
    assert(list.front() == -1);

    // 空列表 pop 测试
    TS::list<int> empty_list;
    bool back_exception_thrown = false;
    bool front_exception_thrown = false;

    try
    {
        empty_list.pop_back();
    }
    catch (const std::range_error &)
    {
        back_exception_thrown = true;
    }

    try
    {
        empty_list.pop_front();
    }
    catch (const std::range_error &)
    {
        front_exception_thrown = true;
    }

    assert(back_exception_thrown);
    assert(front_exception_thrown);

    std::cout << "All push/pop tests passed!" << std::endl;
}

void test_resize()
{
    std::cout << "=== Testing resize ===" << std::endl;

    TS::list<int> list{1, 2, 3};

    // 缩小尺寸
    list.resize(2);
    assert(list.size() == 2);
    assert(list.front() == 1);
    assert(list.back() == 2);

    // 扩大尺寸（默认值）
    list.resize(5);
    assert(list.size() == 5);

    // 扩大尺寸（指定值）
    list.resize(7, 42);
    assert(list.size() == 7);

    // 尺寸不变
    list.resize(7);
    assert(list.size() == 7);

    // resize 到 0
    list.resize(0);
    assert(list.empty());

    std::cout << "All resize tests passed!" << std::endl;
}

void test_swap()
{
    std::cout << "=== Testing swap ===" << std::endl;

    TS::list<int> list1{1, 2, 3};
    TS::list<int> list2{4, 5, 6, 7};

    size_t size1 = list1.size();
    size_t size2 = list2.size();

    list1.swap(list2);

    assert(list1.size() == size2);
    assert(list2.size() == size1);
    assert(list1.front() == 4);
    assert(list2.front() == 1);

    std::cout << "All swap tests passed!" << std::endl;
}

void test_edge_cases()
{
    std::cout << "=== Testing edge cases ===" << std::endl;

    // 空列表操作
    {
        TS::list<int> empty_list;
        assert(empty_list.begin() == empty_list.end());
        assert(empty_list.cbegin() == empty_list.cend());

        // 在空列表中插入
        empty_list.insert(empty_list.end(), 1);
        assert(empty_list.size() == 1);
        assert(empty_list.front() == 1);
        assert(empty_list.back() == 1);
    }

    // 单元素列表
    {
        TS::list<int> single_list{42};
        assert(single_list.size() == 1);
        assert(single_list.front() == single_list.back());
        assert(++single_list.begin() == single_list.end());

        // 删除唯一元素
        single_list.erase(single_list.begin());
        assert(single_list.empty());
    }

    // 在开始和结束位置插入
    {
        TS::list<int> list;
        list.insert(list.begin(), 1);
        list.insert(list.end(), 3);
        list.insert(++list.begin(), 2);

        assert(list.size() == 3);
        assert(list.front() == 1);
        assert(list.back() == 3);

        int expected = 1;
        for (int value : list)
        {
            assert(value == expected++);
        }
    }

    // 删除所有元素
    {
        TS::list<int> list{1, 2, 3};
        list.erase(list.begin(), list.end());
        assert(list.empty());
    }

    std::cout << "All edge case tests passed!" << std::endl;
}

void test_string_type()
{
    std::cout << "=== Testing with string type ===" << std::endl;

    TS::list<std::string> list;

    // 插入字符串
    list.insert(list.end(), "hello");
    list.insert(list.end(), "world");
    list.insert(list.end(), "test");

    assert(list.size() == 3);
    assert(list.front() == "hello");
    assert(list.back() == "test");

    // 修改字符串
    list.front() = "modified";
    assert(list.front() == "modified");

    // 使用 emplace
    list.emplace(list.begin(), "emplace");
    assert(list.front() == "emplace");

    std::cout << "All string tests passed!" << std::endl;
}

void test_performance()
{
    std::cout << "=== Testing performance (large data set) ===" << std::endl;

    const int N = 1000;
    TS::list<int> list;

    // 大量插入
    for (int i = 0; i < N; ++i)
    {
        list.push_back(i);
    }
    assert(list.size() == N);
    assert(list.front() == 0);
    assert(list.back() == N - 1);

    // 大量删除
    list.erase(list.begin(), list.end());
    assert(list.empty());

    std::cout << "Performance test passed!" << std::endl;
}

void test_iterator_validity()
{
    std::cout << "=== Testing iterator validity ===" << std::endl;

    TS::list<int> list{1, 2, 3, 4, 5};

    // 插入后迭代器有效性
    auto it = list.begin();
    ++it;                // 指向第二个元素
    list.insert(it, 10); // 在第二个元素前插入
    assert(*it == 2);    // 原迭代器应仍然有效

    // 删除后迭代器有效性
    it = list.begin();
    ++it; // 指向第二个元素（现在是10）
    auto next_it = list.erase(it);
    assert(*next_it == 2); // 返回下一个元素的迭代器

    std::cout << "Iterator validity test passed!" << std::endl;
}

void test_assign()
{
    std::cout << "=== Testing assign ===" << std::endl;

    TS::list<int> list;

    // assign 初始化列表
    list.assign({1, 2, 3, 4, 5});
    assert(list.size() == 5);
    assert(list.front() == 1);
    assert(list.back() == 5);

    // assign 空列表
    list.assign({});
    assert(list.empty());

    std::cout << "All assign tests passed!" << std::endl;
}

void run_all_tests()
{
    std::cout << "Starting comprehensive TS::list tests..." << std::endl;

    test_constructors_and_destructor();
    test_assignment_operators();
    test_element_access();
    test_iterators();
    test_capacity();
    test_modifiers();
    test_push_pop_operations();
    test_resize();
    test_swap();
    test_edge_cases();
    test_string_type();
    test_performance();
    test_iterator_validity();
    test_assign();

    std::cout << "\n🎉 All TS::list tests passed successfully! 🎉" << std::endl;
}

} // namespace TS_Test

int main()
{
    try
    {
        TS_Test::run_all_tests();
        std::cout << "\nAll tests completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Test failed with unknown exception!" << std::endl;
        return 1;
    }
}