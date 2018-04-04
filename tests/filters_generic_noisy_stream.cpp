#include "noisy.h"
#include <stream.h>
#include <filters_lib.h>
#include <list>
#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using stream::Stream;
using namespace stream::filters;


TEST(FILTERS_GENERIC_NOISY_STREAM, REF_CONTAINER)
{
    std::list<Noisy> list;

    for (int i = 1; i < 11; ++i)
    {
        list.emplace_back(Noisy(i));
    }

    auto result = Stream(list)
                  | filter([](auto &&v){ return v.value > 2; })
                  | map([](auto &&v){ v.value = v.value * v.value; return std::move(v); })
                  | map([](auto &&v){ ++v.value; return std::move(v); })
                  | filter([](auto &&v) { return v.value > 25; })
                  | to_vector();

    ASSERT_EQ(result.size(), 6U);

    ASSERT_EQ(result[0].copyCount, 1);
    ASSERT_EQ(result[0].value, 26);

    ASSERT_EQ(result[1].copyCount, 1);
    ASSERT_EQ(result[1].value, 37);

    ASSERT_EQ(result[2].copyCount, 1);
    ASSERT_EQ(result[2].value, 50);

    ASSERT_EQ(result[3].copyCount, 1);
    ASSERT_EQ(result[3].value, 65);

    ASSERT_EQ(result[4].copyCount, 1);
    ASSERT_EQ(result[4].value, 82);

    ASSERT_EQ(result[5].copyCount, 1);
    ASSERT_EQ(result[5].value, 101);
}

TEST(FILTERS_GENERIC_NOISY_STREAM, CONTAINER)
{
    std::list<Noisy> list;

    for (int i = 1; i < 11; ++i)
    {
        list.emplace_back(Noisy(i));
    }

    auto result = Stream(std::move(list))
                  | filter([](auto &&v){ return v.value > 2; })
                  | map([](auto &&v){ v.value = v.value * v.value; return std::move(v); })
                  | map([](auto &&v){ ++v.value; return std::move(v); })
                  | filter([](auto &&v) { return v.value > 25; })
                  | to_vector();

    ASSERT_EQ(result.size(), 6U);

    ASSERT_EQ(result[0].copyCount, 0);
    ASSERT_EQ(result[0].value, 26);

    ASSERT_EQ(result[1].copyCount, 0);
    ASSERT_EQ(result[1].value, 37);

    ASSERT_EQ(result[2].copyCount, 0);
    ASSERT_EQ(result[2].value, 50);

    ASSERT_EQ(result[3].copyCount, 0);
    ASSERT_EQ(result[3].value, 65);

    ASSERT_EQ(result[4].copyCount, 0);
    ASSERT_EQ(result[4].value, 82);

    ASSERT_EQ(result[5].copyCount, 0);
    ASSERT_EQ(result[5].value, 101);

    auto s = Stream(std::move(list))
               | filter([](auto &&v){ return v.value > 2; })
               | map([](auto &&v){ v.value = v.value * v.value; return std::move(v); })
               | map([](auto &&v){ ++v.value; return std::move(v); })
               | filter([](auto &&v) { return v.value > 25; })
               | sum();

    ASSERT_EQ(s.copyCount, 0);
    ASSERT_EQ(s.value, 0);

    result = Stream(std::move(list))
             | filter([](auto &&v){ return v.value > 2; })
             | map([](auto &&v){ v.value = v.value * v.value; return std::move(v); })
             | map([](auto &&v){ ++v.value; return std::move(v); })
             | filter([](auto &&v) { return v.value > 25; })
             | to_vector();
    ASSERT_TRUE(result.empty());
}

TEST(FILTERS_GENERIC_NOISY_STREAM, GENERATOR)
{
    auto result = Stream([i = int{}]() mutable { return Noisy(++i); })
                  | skip(20)
                  | filter([](auto &&v){ return (v.value % 2 != 0); })
                  | get(20)
                  | to_vector();

    ASSERT_EQ(result.size(), 20U);
    for (int i = 0; i < 20; ++i)
    {
        ASSERT_THAT(result[i].copyCount, 0);
        ASSERT_THAT(result[i].value, 2 * i + 21);
    }
}

TEST(FILTERS_GENERIC_NOISY_STREAM, RANGE)
{
    std::list<Noisy> list;

    for (int i = 1; i < 11; ++i)
    {
        list.emplace_back(Noisy(i));
    }

    auto result = Stream(list.cbegin(), list.cend())
                  | filter([](auto &&v){ return v.value > 2; })
                  | map([](auto &&v){ v.value = v.value * v.value; return std::move(v); })
                  | map([](auto &&v){ ++v.value; return std::move(v); })
                  | filter([](auto &&v) { return v.value > 25; })
                  | to_vector();

    ASSERT_EQ(result.size(), 6U);

    ASSERT_EQ(result[0].copyCount, 1);
    ASSERT_EQ(result[0].value, 26);

    ASSERT_EQ(result[1].copyCount, 1);
    ASSERT_EQ(result[1].value, 37);

    ASSERT_EQ(result[2].copyCount, 1);
    ASSERT_EQ(result[2].value, 50);

    ASSERT_EQ(result[3].copyCount, 1);
    ASSERT_EQ(result[3].value, 65);

    ASSERT_EQ(result[4].copyCount, 1);
    ASSERT_EQ(result[4].value, 82);

    ASSERT_EQ(result[5].copyCount, 1);
    ASSERT_EQ(result[5].value, 101);
}

TEST(FILTERS_GENERIC_NOISY_STREAM, STREAM_FROM_STREAM)
{
    std::ostringstream out;

    Stream a = Stream([i = int{}]() mutable { return Noisy(++i); })
               | group(5)
               | map([](auto &&vec){
                   auto result = std::move(vec[0]);
                   for (unsigned i = 1; i < vec.size(); ++i)
                   {
                       result = result + vec[i];
                   }
                   return result;
               });

    Stream b = a | filter([](auto &&v){ return (v.value % 4 != 0); });
    Stream c = b | get(10);

    auto result = c | to_vector();

    ASSERT_EQ(result.size(), 10U);

    ASSERT_EQ(result[0].copyCount, 0);
    ASSERT_EQ(result[0].value, 15);

    ASSERT_EQ(result[1].copyCount, 0);
    ASSERT_EQ(result[1].value, 65);

    ASSERT_EQ(result[2].copyCount, 0);
    ASSERT_EQ(result[2].value, 90);

    ASSERT_EQ(result[3].copyCount, 0);
    ASSERT_EQ(result[3].value, 115);

    ASSERT_EQ(result[4].copyCount, 0);
    ASSERT_EQ(result[4].value, 165);

    ASSERT_EQ(result[5].copyCount, 0);
    ASSERT_EQ(result[5].value, 190);

    ASSERT_EQ(result[6].copyCount, 0);
    ASSERT_EQ(result[6].value, 215);

    ASSERT_EQ(result[7].copyCount, 0);
    ASSERT_EQ(result[7].value, 265);

    ASSERT_EQ(result[8].copyCount, 0);
    ASSERT_EQ(result[8].value, 290);

    ASSERT_EQ(result[9].copyCount, 0);
    ASSERT_EQ(result[9].value, 315);
}
