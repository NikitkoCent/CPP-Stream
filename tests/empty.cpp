#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <list>
#include <initializer_list>
#include <iostream>
#include <stream.h>
#include <visitor.h>
#include <detail/lazy_invoker.h>

using namespace stream::detail;

TEST(A, 1)
{
    std::vector<int> v;

    stream::Stream stream(v.begin(), v.end());

    ASSERT_EQ(stream.begin(), v.begin());
}

TEST(A, 2)
{
    auto lambda = [](){ return 10; };
    stream::Stream<int, decltype(lambda)> stream(::std::move(lambda));

    auto iter = stream.begin();
    std::cout << *iter << std::endl;
}

TEST(A, 3)
{
    std::vector<int> v{1, 2, 3, 4, 5};

    stream::Stream stream(v);

    for (const auto &val : v)
    {
        std::cout << val << std::endl;
    }
}

TEST(A, 4)
{
    std::vector<int> v{1, 2, 3, 4, 5};

    stream::Stream stream(::std::move(v));

    for (const auto &val : v)
    {
        std::cout << val << std::endl;
    }

    for (const auto &val : stream)
    {
        std::cout << val << std::endl;
    }
}

TEST(A, 5)
{
    auto list = {435, 5, 46,1123};

    stream::Stream stream(list);

    for (const auto &val : stream)
    {
        std::cout << val << std::endl;
    }
}


TEST(A, 6)
{
    stream::Stream stream{435, 5, 46, 1123};

    for (const auto &val : stream)
    {
        std::cout << val << std::endl;
    }
}


TEST(A, 7)
{
    stream::Stream stream(1, 2, 3, 4);

    for (const auto &val : stream)
    {
        std::cout << val << std::endl;
    }
}

TEST(A, 8)
{
    auto l1 = [](){ std::cout << __func__ << std::endl; return 10; };
    auto l2 = [](){ std::cout << __func__ << std::endl; };

    LazyInvoker<decltype(l1)>{l1};
    LazyInvoker<decltype(l2)>{l2};

    int a = LazyInvoker<decltype(l1)>{l1};

    ASSERT_EQ(a, 10);
}


TEST(A, 9)
{
    auto l1 = [](){ std::cout << __func__ << std::endl; return 10; };

    int a = LazyInvoker<decltype(l1)>{l1};

    ASSERT_EQ(a, 10);
}


TEST(A, 10)
{
    auto l1 = [](){ std::cout << __func__ << std::endl; return 10; };
    auto l2 = [](){ std::cout << __func__ << std::endl; };

    int a = LazyInvoker<decltype(l2)>{l2} | l2 | l1 + 5;

    ASSERT_EQ(a, 10);
}
