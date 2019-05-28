#include <stream/stream.h>
#include <stream/operations.h>
#include <list>
#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using stream::Stream;
using namespace stream::ops;


TEST(FILTERS_GENERIC_INT_STREAM, REF_CONTAINER)
{
    std::list<int> list{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    ASSERT_THAT(Stream(list)
                | filter([](auto &&v){ return v > 2; })
                | map([](auto &&v){ return v * v; })
                | map([](auto &&v){ return v + 1; })
                | filter([](auto &&v) { return v > 25; })
                | to_vector(), testing::ElementsAre(26, 37, 50, 65, 82, 101));

    ASSERT_THAT(Stream(list)
                | filter([](auto &&v){ return v > 2; })
                | map([](auto &&v){ return v * v; })
                | map([](auto &&v){ return v + 1; })
                | filter([](auto &&v) { return v > 25; })
                | sum(), 26 + 37 + 50 + 65 + 82 + 101);
}

TEST(FILTERS_GENERIC_INT_STREAM, CONTAINER)
{
    std::list<int> list{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    ASSERT_THAT(Stream(std::move(list))
                | filter([](auto &&v){ return v > 2; })
                | map([](auto &&v){ return v * v; })
                | map([](auto &&v){ return v + 1; })
                | filter([](auto &&v) { return v > 25; })
                | to_vector(), testing::ElementsAre(26, 37, 50, 65, 82, 101));

    ASSERT_THAT(Stream(std::move(list))
                | filter([](auto &&v){ return v > 2; })
                | map([](auto &&v){ return v * v; })
                | map([](auto &&v){ return v + 1; })
                | filter([](auto &&v) { return v > 25; })
                | sum(), 0);

    ASSERT_THAT(Stream(std::move(list))
                | filter([](auto &&v){ return v > 2; })
                | map([](auto &&v){ return v * v; })
                | map([](auto &&v){ return v + 1; })
                | filter([](auto &&v) { return v > 25; })
                | to_vector(), testing::ElementsAre());
}

TEST(FILTERS_GENERIC_INT_STREAM, GENERATOR)
{
    ASSERT_THAT(Stream([i = int{}]() mutable { return ++i; })
                | skip(20)
                | filter([](auto &&v){ return v % 2; })
                | get(20)
                | reduce([](auto &&v){ return std::to_string(v); },
                         [](auto &&str, auto &&v){ return std::move(str += " " + std::to_string(v)); }),
                "21 23 25 27 29 31 33 35 37 39 41 43 45 47 49 51 53 55 57 59");
}

TEST(FILTERS_GENERIC_INT_STREAM, RANGE)
{
    std::list<int> list{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    ASSERT_THAT(Stream(list.cbegin(), list.cend())
                | filter([](auto &&v){ return v > 2; })
                | map([](auto &&v){ return v * v; })
                | map([](auto &&v){ return v + 1; })
                | filter([](auto &&v) { return v > 25; })
                | to_vector(), testing::ElementsAre(26, 37, 50, 65, 82, 101));

    ASSERT_THAT(Stream(list.cbegin(), list.cend())
                | filter([](auto &&v){ return v > 2; })
                | map([](auto &&v){ return v * v; })
                | map([](auto &&v){ return v + 1; })
                | filter([](auto &&v) { return v > 25; })
                | sum(), 26 + 37 + 50 + 65 + 82 + 101);
}

TEST(FILTERS_GENERIC_INT_STREAM, STREAM_FROM_STREAM)
{
    std::ostringstream out;

    Stream a = Stream([i = int{}]() mutable { return ++i; })
                | group(5)
                | map([](auto &&vec){ return std::accumulate(vec.begin(), vec.end(), 0); });

    Stream b = a | filter([](auto &&v){ return (v % 4 != 0); });
    Stream c = b | map([](auto &&v){ return std::to_string(v); });
    c | get(10) | print_to(out);

    ASSERT_EQ(out.str(), "15 65 90 115 165 190 215 265 290 315");
}
