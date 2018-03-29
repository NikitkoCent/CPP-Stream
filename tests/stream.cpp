#include <stream.h>
#include <stream_visitors.h>
#include <vector>
#include <list>
#include <initializer_list>
#include <type_traits>
#include <string>
#include <random>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>


#define LOG std::cout << __FILE__ << " (" << __LINE__ << "): " << __PRETTY_FUNCTION__ << std::endl

using namespace stream;
using namespace stream::visitors;


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
    auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), std::initializer_list<int>>::value));
    Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_RREF)
{
    //const auto l = {1, 2, 3};
    const std::initializer_list<int> l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), const std::initializer_list<int>>::value));
    Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_LREF)
{
    auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), std::initializer_list<int>>::value));
    Stream stream(l);
    ASSERT_TRUE((std::is_same<decltype(stream), Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_LREF)
{
    //const auto l = {1, 2, 3};
    const std::initializer_list<int> l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), const std::initializer_list<int>>::value));
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


TEST(A, A)
{
    stream::Stream{1, 2, 3} | skip(1) | print_to(::std::cout);
    std::cout << std::endl;
}

TEST(A, B)
{
    Stream a = Stream(1, 2, 3) | skip(1);
    Stream b = a | skip(1);

    (b | print_to(std::cout)) << std::endl;
}

TEST(A, C)
{
    stream::Stream{1, 2, 3, 4, 5}
    | skip(1)
    | map([](auto &&v) { return v * v; })
    | map([](auto &&v) { return v * v * v; })
    | print_to(std::cout, "\n");

    std::cout << std::endl;
}

TEST(A, D)
{
    stream::Stream(std::rand)
    | map([](auto &&v) { std::cout << "MAP ^ 2" << std::endl; return v * v; })
    | map([](auto &&v) { std::cout << "MAP ^ 3" << std::endl; return v * v * v; })
    | get(10)
    | skip(1)
    | print_to(std::cout, "\n");

    std::cout << std::endl;
}

TEST(A, E)
{
    int start = 0;

    auto s = Stream([start]() mutable { return start += 10; })
    | map([](auto &&v) { return v * v; })
    | map([](auto &&v) { return v * v * v; })
    | get(10)
    | skip(1)
    | nth(5);

    std::cout << s << std::endl;
}


TEST(A, F)
{
    int start = 0;

    auto vector = Stream([start]() mutable { return start = (start + 10) % 30; })
             | map([](auto &&v) { return v * v; })
             | map([](auto &&v) { return v * v * v; })
             | get(10)
             | to_vector();

    for (const auto &v : vector)
    {
        std::cout << v << ' ';
    }

    std::cout << std::endl;
}

TEST(A, G)
{
    int start = 0;

    auto vector = Stream([start]() mutable { return start += 2; })
                  | filter([](auto &&v) { return (v <= 20); })
                  | map([](auto &&v) { return v * v; })
                  | map([](auto &&v) { return v * v * v; })
                  | get(10)
                  | to_vector();

    for (const auto &v : vector)
    {
        std::cout << v << ' ';
    }

    std::cout << std::endl;
}


TEST(A, I)
{
    int start = 0;
    auto vectors = Stream([start]() mutable { return ++start; })
                    | map([](auto &&v) { return v * v; })
                    | group(10)
                    | get(2)
                    | to_vector();

    for (const auto &vector : vectors)
    {
        for (const auto &elem : vector)
        {
            std::cout << elem << ' ';
        }

        std::cout << std::endl;
    }
}


TEST(A, J)
{
    int a = 0;
    stream::Stream([&a]() -> auto& {return a;})
        | filter([](auto &&) { return true; })
        | map([](auto &&x) -> auto& { return ++x; })
        | get(5)
        | print_to(std::cout);

    std::cout << std::endl;
}

TEST(A, K)
{
    auto vec = stream::Stream([a = 0]() mutable { return std::make_unique<int>(a++); })
        | filter([](auto &&value) { return (*value > 2); })
        | map([](auto &&value) { ++(*value); return std::move(value); })
        | get(5)
        | to_vector();

    for (const auto &v : vec)
    {
        std::cout << *v << ' ';
    }

    std::cout << std::endl;
}


TEST(A, L)
{
    auto value = stream::Stream([a = 0]() mutable { return std::make_unique<int>(a++); })
               | filter([](auto &&value) { return (*value > 2); })
               | map([](auto &&value) { ++(*value); return std::move(value); })
               | get(5)
               | reduce([](auto &&init) { return *init; },
                        [](auto &&v1, auto &&v2) { return v1 + *v2; });

    std::cout << value << std::endl;
}


#undef LOG
