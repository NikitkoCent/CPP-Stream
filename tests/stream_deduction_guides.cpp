#include <stream.h>
#include <filters_lib.h>
#include <vector>
#include <list>
#include <initializer_list>
#include <type_traits>
#include <string>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::filters;


TEST(DEDUCTION_GUIDES_CONTAINER, RREF)
{
    std::list<std::string> c{"one", "two", "three"};

    Stream stream(std::move(c));

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, std::list<std::string>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST)
{
    Stream stream({1, 2, 3});
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_RREF)
{
    std::initializer_list<int> l = {1, 2, 3};

    Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_RREF)
{
    const std::initializer_list<int> l = {1, 2, 3};

    Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_LREF)
{
    std::initializer_list<int> l = {1, 2, 3};

    Stream stream(l);
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_LREF)
{
    const std::initializer_list<int> l = {1, 2, 3};

    Stream stream(l);
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, TEMPLATE_PACK1)
{
    Stream stream1(10.5, 14, 16.0);
    Stream stream2(10.5);

    ASSERT_TRUE((std::is_same<decltype(stream1), Stream<double>>::value));
    ASSERT_TRUE((std::is_same<decltype(stream2), Stream<double>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, TEMPLATE_PACK2)
{
    double a = 10.5;

    Stream stream1(a, 14, 16.0);
    Stream stream2(a);

    ASSERT_TRUE((std::is_same<decltype(stream1), Stream<double>>::value));
    ASSERT_TRUE((std::is_same<decltype(stream2), Stream<double>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, TEMPLATE_PACK3)
{
    double a = 10.5;

    Stream stream1(10.5, a, 16.0);
    Stream stream2(a);

    ASSERT_TRUE((std::is_same<decltype(stream1), Stream<double>>::value));
    ASSERT_TRUE((std::is_same<decltype(stream2), Stream<double>>::value));
}


TEST(DEDUCTION_GUIDES_CONTAINER, STD_STRING)
{
    Stream stream1(std::string("one"), std::string("two"), std::string("three"));
    Stream stream2(std::string("one"));

    ASSERT_TRUE((std::is_same<decltype(stream1), Stream<std::string>>::value));
    ASSERT_TRUE((std::is_same<decltype(stream2), Stream<char, std::string>>::value));
}


TEST(DEDUCTION_GUIDES_CREF_CONTAINER, LREF)
{
    std::list<std::string> c{"one", "two", "three"};

    Stream stream(c);

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, const std::list<std::string>&>>::value));
}

TEST(DEDUCTION_GUIDES_CREF_CONTAINER, CONST_LREF)
{
    const std::list<std::string> c{"one", "two", "three"};

    Stream stream(c);

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, const std::list<std::string>&>>::value));
}

TEST(DEDUCTION_GUIDES_CREF_CONTAINER, CONST_RREF)
{
    const std::list<std::string> c{"one", "two", "three"};

    Stream stream(std::move(c));

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, const std::list<std::string>&>>::value));
}


TEST(DEDUCTION_GUIDES_GENERATOR, C_FUNCTION)
{
    Stream stream1(std::rand);
    Stream<int, decltype(std::rand)> stream2(std::rand);

    ASSERT_TRUE((std::is_same<decltype(stream1), decltype(stream2)>::value));
}

TEST(DEDUCTION_GUIDES_GENERATOR, LAMBDA)
{
    auto lambda = [] { return 10.5; };

    Stream stream1(lambda);
    Stream stream2(std::move(lambda));

    Stream<double, decltype(lambda)> stream3(lambda);

    ASSERT_TRUE((std::is_same<decltype(stream1), decltype(stream3)>::value));
    ASSERT_TRUE((std::is_same<decltype(stream2), decltype(stream3)>::value));
}


TEST(DEDUCTION_GUIDES_RANGE, ITERATOR_LREF_LREF)
{
    std::vector<std::string> vec{"first", "second", "third"};
    auto b = vec.begin();
    auto e = vec.end();

    Stream stream(b, e);

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, typename std::vector<std::string>::iterator>>::value));
}

TEST(DEDUCTION_GUIDES_RANGE, ITERATOR_LREF_RREF)
{
    std::vector<std::string> vec{"first", "second", "third"};
    auto b = vec.begin();
    auto e = vec.end();

    Stream stream(b, std::move(e));

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, typename std::vector<std::string>::iterator>>::value));
}

TEST(DEDUCTION_GUIDES_RANGE, ITERATOR_RREF_LREF)
{
    std::vector<std::string> vec{"first", "second", "third"};
    auto b = vec.begin();
    auto e = vec.end();

    Stream stream(std::move(b), e);

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, typename std::vector<std::string>::iterator>>::value));
}

TEST(DEDUCTION_GUIDES_RANGE, ITERATOR_RREF_RREF)
{
    std::vector<std::string> vec{"first", "second", "third"};
    auto b = vec.begin();
    auto e = vec.end();

    Stream stream(std::move(b), std::move(e));

    ASSERT_TRUE((std::is_same<decltype(stream), Stream<std::string, typename std::vector<std::string>::iterator>>::value));
}
