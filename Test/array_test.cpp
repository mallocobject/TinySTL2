#include "ts_array.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// 测试结果计数器
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(expr)                                                                                 \
    do                                                                                             \
    {                                                                                              \
        if (expr)                                                                                  \
        {                                                                                          \
            std::cout << "PASS: " << #expr << std::endl;                                           \
            tests_passed++;                                                                        \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            std::cout << "FAIL: " << #expr << " (line " << __LINE__ << ")" << std::endl;           \
            tests_failed++;                                                                        \
        }                                                                                          \
    } while (0)

#define TEST_THROW(expr, exception_type)                                                           \
    do                                                                                             \
    {                                                                                              \
        try                                                                                        \
        {                                                                                          \
            expr;                                                                                  \
            std::cout << "FAIL: Expected " << #exception_type << " but no exception thrown (line " \
                      << __LINE__ << ")" << std::endl;                                             \
            tests_failed++;                                                                        \
        }                                                                                          \
        catch (const exception_type &)                                                             \
        {                                                                                          \
            std::cout << "PASS: " << #expr << " throws " << #exception_type << std::endl;          \
            tests_passed++;                                                                        \
        }                                                                                          \
        catch (...)                                                                                \
        {                                                                                          \
            std::cout << "FAIL: Expected " << #exception_type                                      \
                      << " but different exception thrown (line " << __LINE__ << ")" << std::endl; \
            tests_failed++;                                                                        \
        }                                                                                          \
    } while (0)

// 打印测试结果摘要
void print_test_summary()
{
    std::cout << "\n=== TEST SUMMARY ===" << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    std::cout << "Total tests: " << (tests_passed + tests_failed) << std::endl;
}

// 自定义类型用于测试
struct TestType
{
    int value;
    TestType() : value(0)
    {
    }
    TestType(int v) : value(v)
    {
    }
    bool operator==(const TestType &other) const
    {
        return value == other.value;
    }
    bool operator!=(const TestType &other) const
    {
        return value != other.value;
    }
};

std::ostream &operator<<(std::ostream &os, const TestType &tt)
{
    os << "TestType(" << tt.value << ")";
    return os;
}

// 测试用例函数
void test_default_constructor()
{
    std::cout << "\n=== Testing default constructor ===" << std::endl;

    TS::array<int, 5> arr;
    TEST(arr.size() == 5);
    TEST(arr.empty() == false);

    // 检查所有元素是否已默认初始化
    for (std::size_t i = 0; i < arr.size(); ++i)
    {
        TEST(arr[i] == int{});
    }

    // 测试自定义类型
    TS::array<TestType, 3> custom_arr;
    for (std::size_t i = 0; i < custom_arr.size(); ++i)
    {
        TEST(custom_arr[i].value == 0);
    }
}

void test_initializer_list_constructor()
{
    std::cout << "\n=== Testing initializer list constructor ===" << std::endl;

    // 正常初始化
    TS::array<int, 5> arr = {1, 2, 3, 4, 5};
    TEST(arr.size() == 5);
    TEST(arr[0] == 1);
    TEST(arr[1] == 2);
    TEST(arr[2] == 3);
    TEST(arr[3] == 4);
    TEST(arr[4] == 5);

    // 部分初始化
    TS::array<int, 5> arr2 = {1, 2, 3};
    TEST(arr2[0] == 1);
    TEST(arr2[1] == 2);
    TEST(arr2[2] == 3);
    TEST(arr2[3] == 0); // 应该被默认初始化
    TEST(arr2[4] == 0); // 应该被默认初始化

    // 空初始化列表
    TS::array<int, 3> arr3 = {};
    TEST(arr3[0] == 0);
    TEST(arr3[1] == 0);
    TEST(arr3[2] == 0);

    // 测试异常 - 初始化列表太大
    TEST_THROW((TS::array<int, 2>{1, 2, 3}), std::out_of_range);

    // 测试自定义类型
    TS::array<TestType, 4> custom_arr = {TestType(1), TestType(2), TestType(3)};
    TEST(custom_arr[0].value == 1);
    TEST(custom_arr[1].value == 2);
    TEST(custom_arr[2].value == 3);
    TEST(custom_arr[3].value == 0); // 默认初始化
}

void test_copy_constructor()
{
    std::cout << "\n=== Testing copy constructor ===" << std::endl;

    TS::array<int, 5> original = {1, 2, 3, 4, 5};
    TS::array<int, 5> copy = original;

    TEST(copy.size() == original.size());
    for (std::size_t i = 0; i < original.size(); ++i)
    {
        TEST(copy[i] == original[i]);
    }

    // 修改拷贝不应影响原始数组
    copy[0] = 100;
    TEST(original[0] == 1);
    TEST(copy[0] == 100);

    // 测试自定义类型
    TS::array<TestType, 3> orig_custom = {TestType(1), TestType(2), TestType(3)};
    TS::array<TestType, 3> copy_custom = orig_custom;

    for (std::size_t i = 0; i < orig_custom.size(); ++i)
    {
        TEST(copy_custom[i].value == orig_custom[i].value);
    }

    copy_custom[0].value = 100;
    TEST(orig_custom[0].value == 1);
    TEST(copy_custom[0].value == 100);
}

void test_move_constructor()
{
    std::cout << "\n=== Testing move constructor ===" << std::endl;

    TS::array<std::string, 3> original = {"one", "two", "three"};
    TS::array<std::string, 3> moved = std::move(original);

    TEST(moved.size() == 3);
    TEST(moved[0] == "one");
    TEST(moved[1] == "two");
    TEST(moved[2] == "three");

    // 移动后原始数组的状态是未定义的，但应该仍然可以访问
    // 这里我们只检查大小，因为元素状态可能不确定
    TEST(original.size() == 3);

    // 测试自定义类型
    TS::array<TestType, 2> orig_custom = {TestType(1), TestType(2)};
    TS::array<TestType, 2> moved_custom = std::move(orig_custom);

    TEST(moved_custom[0].value == 1);
    TEST(moved_custom[1].value == 2);
    TEST(orig_custom.size() == 2);
}

void test_copy_assignment()
{
    std::cout << "\n=== Testing copy assignment ===" << std::endl;

    TS::array<int, 5> original = {1, 2, 3, 4, 5};
    TS::array<int, 5> copy;
    copy = original;

    TEST(copy.size() == original.size());
    for (std::size_t i = 0; i < original.size(); ++i)
    {
        TEST(copy[i] == original[i]);
    }

    // 修改拷贝不应影响原始数组
    copy[0] = 100;
    TEST(original[0] == 1);
    TEST(copy[0] == 100);

    // 自赋值测试
    copy = copy;
    TEST(copy[0] == 100); // 应该保持不变

    // 测试自定义类型
    TS::array<TestType, 3> orig_custom = {TestType(1), TestType(2), TestType(3)};
    TS::array<TestType, 3> copy_custom;
    copy_custom = orig_custom;

    for (std::size_t i = 0; i < orig_custom.size(); ++i)
    {
        TEST(copy_custom[i].value == orig_custom[i].value);
    }

    copy_custom[0].value = 100;
    TEST(orig_custom[0].value == 1);
    TEST(copy_custom[0].value == 100);
}

void test_move_assignment()
{
    std::cout << "\n=== Testing move assignment ===" << std::endl;

    TS::array<std::string, 3> original = {"one", "two", "three"};
    TS::array<std::string, 3> moved;
    moved = std::move(original);

    TEST(moved.size() == 3);
    TEST(moved[0] == "one");
    TEST(moved[1] == "two");
    TEST(moved[2] == "three");

    // 移动后原始数组的状态是未定义的，但应该仍然可以访问
    // 这里我们只检查大小，因为元素状态可能不确定
    TEST(original.size() == 3);

    // 自赋值测试
    moved = std::move(moved);
    TEST(moved[0] == "one"); // 应该保持不变

    // 测试自定义类型
    TS::array<TestType, 2> orig_custom = {TestType(1), TestType(2)};
    TS::array<TestType, 2> moved_custom;
    moved_custom = std::move(orig_custom);

    TEST(moved_custom[0].value == 1);
    TEST(moved_custom[1].value == 2);
    TEST(orig_custom.size() == 2);
}

void test_element_access()
{
    std::cout << "\n=== Testing element access ===" << std::endl;

    TS::array<int, 5> arr = {1, 2, 3, 4, 5};

    // operator[]
    TEST(arr[0] == 1);
    TEST(arr[4] == 5);

    // at()
    TEST(arr.at(0) == 1);
    TEST(arr.at(4) == 5);

    // 测试 at() 异常
    TEST_THROW(arr.at(5), std::out_of_range);
    TEST_THROW(arr.at(-1), std::out_of_range); // -1 会转换为非常大的无符号数

    // front() 和 back()
    TEST(arr.front() == 1);
    TEST(arr.back() == 5);

    // data()
    TEST(arr.data() == &arr[0]);
    TEST(*arr.data() == 1);

    // 空数组测试
    TS::array<int, 0> empty_arr;
    TEST_THROW(empty_arr.front(), std::logic_error);
    TEST_THROW(empty_arr.back(), std::logic_error);
    // data() 对于空数组不应该抛出异常
    TEST(empty_arr.data() != nullptr);

    // 测试自定义类型
    TS::array<TestType, 3> custom_arr = {TestType(1), TestType(2), TestType(3)};
    TEST(custom_arr[0].value == 1);
    TEST(custom_arr.at(1).value == 2);
    TEST(custom_arr.front().value == 1);
    TEST(custom_arr.back().value == 3);
    TEST(custom_arr.data() == &custom_arr[0]);
}

void test_iterators()
{
    std::cout << "\n=== Testing iterators ===" << std::endl;

    TS::array<int, 5> arr = {1, 2, 3, 4, 5};

    // 普通迭代器
    int sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        sum += *it;
    }
    TEST(sum == 15);

    // 常量迭代器
    sum = 0;
    for (auto it = arr.cbegin(); it != arr.cend(); ++it)
    {
        sum += *it;
    }
    TEST(sum == 15);

    // 基于范围的 for 循环
    sum = 0;
    for (int val : arr)
    {
        sum += val;
    }
    TEST(sum == 15);

    // 修改元素
    for (auto &val : arr)
    {
        val += 1;
    }
    TEST(arr[0] == 2);
    TEST(arr[4] == 6);

    // 空数组迭代器
    TS::array<int, 0> empty_arr;
    TEST(empty_arr.begin() == empty_arr.end());
    TEST(empty_arr.cbegin() == empty_arr.cend());

    // 测试自定义类型
    TS::array<TestType, 3> custom_arr = {TestType(1), TestType(2), TestType(3)};
    sum = 0;
    for (auto it = custom_arr.begin(); it != custom_arr.end(); ++it)
    {
        sum += it->value;
    }
    TEST(sum == 6);

    // 修改自定义类型元素
    for (auto &val : custom_arr)
    {
        val.value += 10;
    }
    TEST(custom_arr[0].value == 11);
    TEST(custom_arr[1].value == 12);
    TEST(custom_arr[2].value == 13);
}

void test_capacity()
{
    std::cout << "\n=== Testing capacity ===" << std::endl;

    TS::array<int, 5> arr;
    TEST(arr.size() == 5);
    TEST(arr.max_size() == 5);
    TEST(arr.empty() == false);

    TS::array<int, 0> empty_arr;
    TEST(empty_arr.size() == 0);
    TEST(empty_arr.max_size() == 0);
    TEST(empty_arr.empty() == true);

    // 测试自定义类型
    TS::array<TestType, 4> custom_arr;
    TEST(custom_arr.size() == 4);
    TEST(custom_arr.max_size() == 4);
    TEST(custom_arr.empty() == false);
}

void test_fill()
{
    std::cout << "\n=== Testing fill ===" << std::endl;

    TS::array<int, 5> arr;
    arr.fill(42);

    for (std::size_t i = 0; i < arr.size(); ++i)
    {
        TEST(arr[i] == 42);
    }

    // 字符串测试
    TS::array<std::string, 3> str_arr;
    str_arr.fill("hello");

    for (std::size_t i = 0; i < str_arr.size(); ++i)
    {
        TEST(str_arr[i] == "hello");
    }

    // 空数组填充测试
    TS::array<int, 0> empty_arr;
    empty_arr.fill(42); // 应该不会崩溃

    // 测试自定义类型
    TS::array<TestType, 3> custom_arr;
    custom_arr.fill(TestType(99));

    for (std::size_t i = 0; i < custom_arr.size(); ++i)
    {
        TEST(custom_arr[i].value == 99);
    }
}

void test_swap()
{
    std::cout << "\n=== Testing swap ===" << std::endl;

    TS::array<int, 3> arr1 = {1, 2, 3};
    TS::array<int, 3> arr2 = {4, 5, 6};

    arr1.swap(arr2);

    TEST(arr1[0] == 4);
    TEST(arr1[1] == 5);
    TEST(arr1[2] == 6);

    TEST(arr2[0] == 1);
    TEST(arr2[1] == 2);
    TEST(arr2[2] == 3);

    // 自交换测试
    arr1.swap(arr1);
    TEST(arr1[0] == 4); // 应该保持不变

    // 使用全局 swap - 交换回来
    // 注意：此时 arr1 = {4, 5, 6}, arr2 = {1, 2, 3}
    // 交换后：arr1 = {1, 2, 3}, arr2 = {4, 5, 6}
    TS::swap(arr1, arr2);
    TEST(arr1[0] == 1);
    TEST(arr1[1] == 2);
    TEST(arr1[2] == 3);

    TEST(arr2[0] == 4);
    TEST(arr2[1] == 5);
    TEST(arr2[2] == 6);

    // 空数组交换测试
    TS::array<int, 0> empty1, empty2;
    empty1.swap(empty2); // 应该不会崩溃

    // 测试自定义类型
    TS::array<TestType, 2> custom1 = {TestType(1), TestType(2)};
    TS::array<TestType, 2> custom2 = {TestType(3), TestType(4)};

    custom1.swap(custom2);
    TEST(custom1[0].value == 3);
    TEST(custom1[1].value == 4);
    TEST(custom2[0].value == 1);
    TEST(custom2[1].value == 2);
}

void test_equality_operator()
{
    std::cout << "\n=== Testing equality operator ===" << std::endl;

    TS::array<int, 3> arr1 = {1, 2, 3};
    TS::array<int, 3> arr2 = {1, 2, 3};
    TS::array<int, 3> arr3 = {4, 5, 6};
    TS::array<int, 3> arr4 = {1, 2, 4};

    // 相等比较
    TEST(arr1 == arr2);
    TEST(!(arr1 == arr3));

    // 空数组比较
    TS::array<int, 0> empty1, empty2;
    TEST(empty1 == empty2);

    // 测试自定义类型
    TS::array<TestType, 2> custom1 = {TestType(1), TestType(2)};
    TS::array<TestType, 2> custom2 = {TestType(1), TestType(2)};
    TS::array<TestType, 2> custom3 = {TestType(3), TestType(4)};

    TEST(custom1 == custom2);
    TEST(!(custom1 == custom3));
}

void test_get_functions()
{
    std::cout << "\n=== Testing get functions ===" << std::endl;

    // 测试非const左值引用
    TS::array<int, 5> arr = {1, 2, 3, 4, 5};
    TEST(TS::get<0>(arr) == 1);
    TEST(TS::get<4>(arr) == 5);

    // 测试修改元素
    TS::get<0>(arr) = 100;
    TEST(arr[0] == 100);

    // 测试const左值引用
    const TS::array<int, 5> const_arr = {1, 2, 3, 4, 5};
    TEST(TS::get<0>(const_arr) == 1);
    TEST(TS::get<4>(const_arr) == 5);

    // 测试非const右值引用
    TS::array<std::string, 3> str_arr = {"one", "two", "three"};
    std::string moved_str = TS::get<0>(std::move(str_arr));
    TEST(moved_str == "one");

    // 测试const右值引用
    const TS::array<std::string, 3> const_str_arr = {"one", "two", "three"};
    const std::string const_moved_str = TS::get<0>(std::move(const_str_arr));
    TEST(const_moved_str == "one");

    // 测试自定义类型
    TS::array<TestType, 3> custom_arr = {TestType(1), TestType(2), TestType(3)};
    TEST(TS::get<0>(custom_arr).value == 1);
    TEST(TS::get<2>(custom_arr).value == 3);

    TS::get<1>(custom_arr).value = 99;
    TEST(custom_arr[1].value == 99);
}

void test_zero_sized_array()
{
    std::cout << "\n=== Testing zero-sized array ===" << std::endl;

    TS::array<int, 0> arr;
    TEST(arr.size() == 0);
    TEST(arr.empty() == true);
    TEST(arr.max_size() == 0);
    TEST(arr.begin() == arr.end());
    TEST(arr.cbegin() == arr.cend());
    TEST(arr.data() != nullptr); // data() 应该返回有效指针

    // 应该抛出异常的操作
    TEST_THROW(arr.front(), std::logic_error);
    TEST_THROW(arr.back(), std::logic_error);
    TEST_THROW(arr.at(0), std::out_of_range);

    // 填充操作应该正常工作（无操作）
    arr.fill(42);

    // 交换操作应该正常工作
    TS::array<int, 0> arr2;
    arr.swap(arr2);

    // get 函数应该无法编译（索引越界），但这里我们无法测试编译错误
    // 可以使用 static_assert 来测试，但需要在编译时进行
}

void test_const_correctness()
{
    std::cout << "\n=== Testing const correctness ===" << std::endl;

    const TS::array<int, 3> arr = {1, 2, 3};

    // 应该能够调用 const 方法
    TEST(arr.size() == 3);
    TEST(arr.empty() == false);
    TEST(arr[0] == 1);
    TEST(arr.at(1) == 2);
    TEST(arr.front() == 1);
    TEST(arr.back() == 3);
    TEST(arr.data() != nullptr);

    // 应该能够使用 const 迭代器
    int sum = 0;
    for (auto it = arr.cbegin(); it != arr.cend(); ++it)
    {
        sum += *it;
    }
    TEST(sum == 6);

    // 应该能够使用基于范围的 for 循环
    sum = 0;
    for (int val : arr)
    {
        sum += val;
    }
    TEST(sum == 6);

    // 测试自定义类型
    const TS::array<TestType, 2> custom_arr = {TestType(1), TestType(2)};
    TEST(custom_arr[0].value == 1);
    TEST(custom_arr.at(1).value == 2);
    TEST(custom_arr.front().value == 1);
    TEST(custom_arr.back().value == 2);
    TEST(custom_arr.data() != nullptr);
}

void test_type_traits()
{
    std::cout << "\n=== Testing type traits ===" << std::endl;

    // 测试类型别名
    TS::array<int, 5> arr;

    TEST((std::is_same<decltype(arr)::value_type, int>::value));
    TEST((std::is_same<decltype(arr)::size_type, std::size_t>::value));
    TEST((std::is_same<decltype(arr)::difference_type, std::ptrdiff_t>::value));
    TEST((std::is_same<decltype(arr)::reference, int &>::value));
    TEST((std::is_same<decltype(arr)::const_reference, const int &>::value));
    TEST((std::is_same<decltype(arr)::pointer, int *>::value));
    TEST((std::is_same<decltype(arr)::const_pointer, const int *>::value));
    TEST((std::is_same<decltype(arr)::iterator, int *>::value));
    TEST((std::is_same<decltype(arr)::const_iterator, const int *>::value));
}

int main()
{
    std::cout << "Starting TS::array tests..." << std::endl;

    // 运行所有测试
    test_default_constructor();
    test_initializer_list_constructor();
    test_copy_constructor();
    test_move_constructor();
    test_copy_assignment();
    test_move_assignment();
    test_element_access();
    test_iterators();
    test_capacity();
    test_fill();
    test_swap();
    test_equality_operator();
    test_get_functions();
    test_zero_sized_array();
    test_const_correctness();
    test_type_traits();

    // 打印测试结果摘要
    print_test_summary();

    // 返回失败测试的数量作为退出代码
    return tests_failed > 0 ? 1 : 0;
}