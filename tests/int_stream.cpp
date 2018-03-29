#include <stream.h>
#include <filters_lib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::filters;


TEST(INT_STREAM, SKIP1)
{
    stream::Stream{1, 2, 3} | skip(1) | print_to(::std::cout);
    std::cout << std::endl;
}

TEST(INT_STREAM, SKIP2)
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
