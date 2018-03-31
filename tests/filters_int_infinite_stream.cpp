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
