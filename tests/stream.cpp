#include <streamv2.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <list>
#include <initializer_list>
#include <type_traits>
#include <string>
#include <random>
#include <iostream>


#define LOG std::cout << __FILE__ << " (" << __LINE__ << "): " << __PRETTY_FUNCTION__ << std::endl


struct OnlyMovable
{
    OnlyMovable() { LOG; }
    OnlyMovable(const OnlyMovable&) = delete;
    OnlyMovable(OnlyMovable&&) { LOG; }

    OnlyMovable& operator=(const OnlyMovable&) = delete;
    OnlyMovable& operator=(OnlyMovable&&) { LOG; return *this; }
};


TEST(DEDUCTION_GUIDES_CONTAINER, RREF)
{
    std::list<std::string> c{"one", "two", "three"};

    stream::Stream stream(std::move(c));

    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<std::string, std::list<std::string>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST)
{
    stream::Stream stream({1, 2, 3});
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_RREF)
{
    auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), std::initializer_list<int>>::value));
    stream::Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_RREF)
{
    const auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), const std::initializer_list<int>>::value));
    stream::Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_LREF)
{
    auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), std::initializer_list<int>>::value));
    stream::Stream stream(l);
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_LREF)
{
    const auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), const std::initializer_list<int>>::value));
    stream::Stream stream(l);
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, TEMPLATE_PACK)
{
    stream::Stream stream1(10.5, 14, 16.0);
    stream::Stream stream2(10.5);

    ASSERT_TRUE((::std::is_same<decltype(stream1), stream::Stream<double>>::value));
    ASSERT_TRUE((::std::is_same<decltype(stream2), stream::Stream<double>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, STD_STRING)
{
    stream::Stream stream1(std::string("one"), std::string("two"), std::string("three"));
    stream::Stream stream2(std::string("one"));

    ASSERT_TRUE((std::is_same<decltype(stream1), stream::Stream<std::string>>::value));
    ASSERT_TRUE((std::is_same<decltype(stream2), stream::Stream<char, std::string>>::value));
}


TEST(DEDUCTION_GUIDES_CREF_CONTAINER, LREF)
{
    std::list<std::string> c{"one", "two", "three"};

    stream::Stream stream(c);

    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<std::string, const std::list<std::string>&>>::value));
}

TEST(DEDUCTION_GUIDES_CREF_CONTAINER, CONST_LREF)
{
    const std::list<std::string> c{"one", "two", "three"};

    stream::Stream stream(c);

    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<std::string, const std::list<std::string>&>>::value));
}

TEST(DEDUCTION_GUIDES_CREF_CONTAINER, CONST_RREF)
{
    const std::list<std::string> c{"one", "two", "three"};

    stream::Stream stream(std::move(c));

    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<std::string, const std::list<std::string>&>>::value));
}


TEST(DEDUCTION_GUIDES_GENERATOR, C_FUNCTION)
{
    stream::Stream stream1(std::rand);
    stream::Stream<int, decltype(std::rand)> stream2(::std::rand);

    ASSERT_TRUE((std::is_same<decltype(stream1), decltype(stream2)>::value));
}

TEST(DEDUCTION_GUIDES_GENERATOR, LAMBDA)
{
    auto lambda = [] { return 10.5; };

    stream::Stream stream1(lambda);
    stream::Stream stream2(std::move(lambda));

    stream::Stream<double, decltype(lambda)> stream3(lambda);

    ASSERT_TRUE((std::is_same<decltype(stream1), decltype(stream3)>::value));
    ASSERT_TRUE((std::is_same<decltype(stream2), decltype(stream3)>::value));
}


#undef LOG
