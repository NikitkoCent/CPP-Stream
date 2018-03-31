#include <stream.h>
#include <filters_lib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


namespace
{
    struct Gen
    {
        int v = 20;

        int operator()()
        {
            return ++v;
        }
    };
}


using namespace stream;
using namespace stream::filters;


TEST(FILTERS_INT_INFINITE_STREAM, GET_0)
{
    ASSERT_TRUE((stream::Stream(Gen()) | get(0) | to_vector()).empty());
}

TEST(FILTERS_INT_INFINITE_STREAM, GET_1)
{
    ASSERT_THAT(stream::Stream(Gen()) | get(1) | to_vector(), testing::ElementsAre(21));
}

TEST(FILTERS_INT_INFINITE_STREAM, GET_10)
{
    ASSERT_THAT(stream::Stream(Gen()) | get(10) | to_vector(),
                testing::ElementsAre(21, 22, 23, 24, 25, 26, 27, 28, 29, 30));
}


TEST(FILTERS_INT_INFINITE_STREAM, SKIP_0)
{
    ASSERT_EQ(stream::Stream(Gen()) | skip(0) | nth(0), 21);
}

TEST(FILTERS_INT_INFINITE_STREAM, SKIP_1)
{
    ASSERT_EQ(stream::Stream(Gen()) | skip(1) | nth(0), 22);
}

TEST(FILTERS_INT_INFINITE_STREAM, SKIP_10)
{
    ASSERT_EQ(stream::Stream(Gen()) | skip(10) | nth(0), 31);
}


TEST(FILTERS_INT_INFINITE_STREAM, MAP1)
{
    ASSERT_THAT(stream::Stream(Gen()) | map([](auto &&v){ return v * v; }) | nth(0), 21 * 21);
}

TEST(FILTERS_INT_INFINITE_STREAM, MAP2)
{
    ASSERT_THAT(stream::Stream(Gen()) | map([](auto &&v){ return v * v; }) | nth(9), 900);
}


TEST(FILTERS_INT_INFINITE_STREAM, FILTER_TRUE)
{
    ASSERT_THAT(stream::Stream(Gen()) | filter([](auto&&){ return true; }) | nth(0), 21);
}

TEST(FILTERS_INT_INFINITE_STREAM, FILTER_10)
{
    ASSERT_THAT(stream::Stream(Gen()) | filter([](auto &&v){ return (v == 31); }) | nth(0), 31);
}

TEST(FILTERS_INT_INFINITE_STREAM, FILTER_ONLY_EVEN)
{
    ASSERT_THAT(stream::Stream(Gen()) | filter([](auto &&v){ return !(v % 2); }) | get(10) | to_vector(),
                testing::ElementsAre(22, 24, 26, 28, 30, 32, 34, 36, 38, 40));
}


TEST(FILTERS_INT_INFINITE_STREAM, GROUP_1)
{
    auto result = stream::Stream(Gen()) | group(1) | get(10) | to_vector();
    ASSERT_EQ(result.size(), 10U);
    for (unsigned i = 0; i < result.size(); ++i)
    {
        ASSERT_THAT(result[i], testing::ElementsAre(21 + i));
    }
}

TEST(FILTERS_INT_INFINITE_STREAM, GROUP_10)
{
    auto result = stream::Stream(Gen()) | group(10) | nth(5);
    ASSERT_THAT(result, testing::ElementsAre(71, 72, 73, 74, 75, 76, 77, 78, 79, 80));
}


TEST(FILTERS_INT_INFINITE_STREAM, NTH_0)
{
    ASSERT_THAT(stream::Stream(Gen()) | nth(0), 21);
}

TEST(FILTERS_INT_INFINITE_STREAM, NTH_100)
{
    ASSERT_THAT(stream::Stream(Gen()) | nth(100), 121);
}
