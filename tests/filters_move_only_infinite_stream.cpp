#include <stream.h>
#include <filters_lib.h>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


namespace
{
    struct Gen
    {
        int v = 20;

        std::unique_ptr<int> operator()()
        {
            return std::make_unique<int>(++v);
        }
    };
}


using namespace stream;
using namespace stream::filters;


TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, GET_0)
{
    ASSERT_TRUE((stream::Stream(Gen()) | get(0) | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, GET_1)
{
    ASSERT_THAT(Stream(Gen()) | get(1) | to_vector(), testing::ElementsAre(testing::Pointee(21)));
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, GET_10)
{
    ASSERT_THAT(Stream(Gen()) | get(10) | to_vector(),
                testing::ElementsAre(testing::Pointee(21),
                                     testing::Pointee(22),
                                     testing::Pointee(23),
                                     testing::Pointee(24),
                                     testing::Pointee(25),
                                     testing::Pointee(26),
                                     testing::Pointee(27),
                                     testing::Pointee(28),
                                     testing::Pointee(29),
                                     testing::Pointee(30)));
}


TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, SKIP_0)
{
    ASSERT_THAT(Stream(Gen()) | skip(0) | nth(0), testing::Pointee(21));
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, SKIP_1)
{
    ASSERT_THAT(Stream(Gen()) | skip(1) | nth(0), testing::Pointee(22));
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, SKIP_10)
{
    ASSERT_THAT(Stream(Gen()) | skip(10) | nth(0), testing::Pointee(31));
}


TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, MAP1)
{
    ASSERT_THAT(Stream(Gen()) | map([](auto &&v){ *v = *v * *v; return std::move(v); }) | nth(0),
                testing::Pointee(21 * 21));
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, MAP2)
{
    ASSERT_THAT(Stream(Gen()) | map([](auto &&v){ *v = *v * *v; return std::move(v); }) | nth(9),
                testing::Pointee(900));
}


TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, FILTER_TRUE)
{
    ASSERT_THAT(Stream(Gen()) | filter([](auto&&){ return true; }) | nth(0), testing::Pointee(21));
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, FILTER_10)
{
    ASSERT_THAT(Stream(Gen()) | filter([](auto &&v){ return (*v == 31); }) | nth(0), testing::Pointee(31));
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, FILTER_ONLY_EVEN)
{
    ASSERT_THAT(Stream(Gen()) | filter([](auto &&v){ return ((*v % 2) == 0); }) | get(10) | to_vector(),
                testing::ElementsAre(testing::Pointee(22),
                                     testing::Pointee(24),
                                     testing::Pointee(26),
                                     testing::Pointee(28),
                                     testing::Pointee(30),
                                     testing::Pointee(32),
                                     testing::Pointee(34),
                                     testing::Pointee(36),
                                     testing::Pointee(38),
                                     testing::Pointee(40)));
}


TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, NTH_0)
{
    ASSERT_THAT(Stream(Gen()) | nth(0), testing::Pointee(21));
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, NTH_100)
{
    ASSERT_THAT(Stream(Gen()) | nth(100), testing::Pointee(121));
}


#if !defined(__GNUG__) || defined(__clang__)
TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, GROUP_1)
{
    auto result = Stream(Gen()) | group(1) | get(10) | to_vector();
    ASSERT_EQ(result.size(), 10U);
    for (unsigned i = 0; i < result.size(); ++i)
    {
        ASSERT_THAT(result[i], testing::ElementsAre(testing::Pointee(21 + i)));
    }
}

TEST(FILTERS_MOVE_ONLY_INFINITE_STREAM, GROUP_10)
{
    auto result = Stream(Gen()) | group(10) | nth(5);
    ASSERT_THAT(result, testing::ElementsAre(testing::Pointee(71),
                                             testing::Pointee(72),
                                             testing::Pointee(73),
                                             testing::Pointee(74),
                                             testing::Pointee(75),
                                             testing::Pointee(76),
                                             testing::Pointee(77),
                                             testing::Pointee(78),
                                             testing::Pointee(79),
                                             testing::Pointee(80)));
}
#endif // #if !defined(__GNUG__) || defined(__clang__)
