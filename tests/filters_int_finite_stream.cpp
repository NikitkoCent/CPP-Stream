#include <stream.h>
#include <operations.h>
#include <vector>
#include <sstream>
#include <type_traits>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::ops;


TEST(FILTERS_INT_FINITE_STREAM, PRINT_TO_RETURNVALUE)
{
    std::ostringstream stream;

    ASSERT_TRUE((std::is_same<decltype(stream::Stream{1, 2, 3} | print_to(stream)), decltype(stream)&>::value));
    ASSERT_EQ(&(stream::Stream{1, 2, 3} | print_to(stream)), &stream);
}

TEST(FILTERS_INT_FINITE_STREAM, PRINT_TO_EMPTY)
{
    std::ostringstream stream;

    stream::Stream<int>() | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_FINITE_STREAM, PRINT_TO_1)
{
    std::ostringstream stream;

    stream::Stream(20) | print_to(stream);
    ASSERT_EQ(stream.str(), "20");
}

TEST(FILTERS_INT_FINITE_STREAM, PRINT_TO_2)
{
    std::ostringstream stream;

    stream::Stream(1, 2) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2");
}

TEST(FILTERS_INT_FINITE_STREAM, PRINT_TO_GENERIC)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5 6 7 8 9");
}
/*


TEST(FILTERS_INT_FINITE_STREAM, SKIP_0)
{
    ASSERT_THAT((stream::Stream{1, 2, 3} | skip(0) | to_vector()), testing::ElementsAre(1, 2, 3));
}

TEST(FILTERS_INT_FINITE_STREAM, SKIP_1)
{
    ASSERT_THAT((stream::Stream{1, 2, 3} | skip(1) | to_vector()), testing::ElementsAre(2, 3));
}

TEST(FILTERS_INT_FINITE_STREAM, SKIP_ALL)
{
    ASSERT_TRUE((stream::Stream{1, 2, 3} | skip(3) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, SKIP_ALL_BUT_ONE)
{
    ASSERT_THAT((stream::Stream{1, 2, 3} | skip(2) | to_vector()), testing::ElementsAre(3));
}

TEST(FILTERS_INT_FINITE_STREAM, SKIP_DOUBLE)
{
    Stream a = Stream(1, 2, 3) | skip(1);
    Stream b = a | skip(1);

    ASSERT_THAT(b | to_vector(), testing::ElementsAre(3));
}


TEST(FILTERS_INT_FINITE_STREAM, MAP_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | map([](auto &&v){ return std::move(v); }) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, MAP_1)
{
    ASSERT_THAT((stream::Stream(15) | map([](auto &&v){ return v * v; }) | to_vector()), testing::ElementsAre(225));
}

TEST(FILTERS_INT_FINITE_STREAM, MAP_GENERIC)
{
    ASSERT_THAT((stream::Stream(-1, 2, -3, 4, -5, 6, -7, 8, -9) | map([](auto &&v){ return v * v ;}) | to_vector()),
                testing::ElementsAre(1, 4, 9, 16, 25, 36, 49, 64, 81));
}


TEST(FILTERS_INT_FINITE_STREAM, GET_0_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | get(0) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, GET_0_NOTEMPTY)
{
    ASSERT_TRUE((stream::Stream(1, 2, 3) | get(0) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, GET_1_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | get(1) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, GET_1_NOTEMPTY)
{
    ASSERT_THAT((stream::Stream(5, 2, 3) | get(1) | to_vector()), testing::ElementsAre(5));
}

TEST(FILTERS_INT_FINITE_STREAM, GET_1_FROM_SINGLE)
{
    ASSERT_THAT((stream::Stream(5) | get(1) | to_vector()), testing::ElementsAre(5));
}

TEST(FILTERS_INT_FINITE_STREAM, GET_ALL)
{
    ASSERT_THAT((stream::Stream(1, 2, 3, 4, 5) | get(5) | to_vector()), testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(FILTERS_INT_FINITE_STREAM, GET_OUT_OF_RANGE)
{
    ASSERT_THAT((stream::Stream(1, 2, 3, 4, 5) | get(6) | to_vector()), testing::ElementsAre(1, 2, 3, 4, 5));
}


auto sumReducer = [](auto &&v1, auto &&v2) { return v1 + v2; };

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_EMPTY)
{
    ASSERT_EQ(stream::Stream<int>() | reduce(sumReducer), 0);
}

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_SINGLE)
{
    ASSERT_EQ(stream::Stream(10) | reduce(sumReducer), 10);
}

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_DOUBLE)
{
    ASSERT_EQ(stream::Stream(45, -100) | reduce(sumReducer), -55);
}

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_GENERIC)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) | reduce(sumReducer), 55);
}

auto stringReducer = [](auto &&str, auto &&num) { return std::move(str += " " + std::to_string(num)); };
auto stringInit = [](auto &&num) { return std::to_string(num); };

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_TO_STRING_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | reduce(stringInit, stringReducer)).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_TO_STRING_SINGLE)
{
    ASSERT_EQ(stream::Stream(10) | reduce(stringInit, stringReducer), "10");
}

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_TO_STRING_DOUBLE)
{
    ASSERT_EQ(stream::Stream(45, -100) | reduce(stringInit, stringReducer), "45 -100");
}

TEST(FILTERS_INT_FINITE_STREAM, REDUCE_TO_STRING_GENERIC)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) | reduce(stringInit, stringReducer),
              "1 2 3 4 5 6 7 8 9 10");
}


TEST(FILTERS_INT_FINITE_STREAM, SUM_EMPTY)
{
    ASSERT_EQ(stream::Stream<int>() | sum(), 0);
}

TEST(FILTERS_INT_FINITE_STREAM, SUM_SINGLE)
{
    ASSERT_EQ(stream::Stream(10) | sum(), 10);
}

TEST(FILTERS_INT_FINITE_STREAM, SUM_DOUBLE)
{
    ASSERT_EQ(stream::Stream(45, -100) | sum(), -55);
}

TEST(FILTERS_INT_FINITE_STREAM, SUM_GENERIC)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) | sum(), 55);
}


TEST(FILTERS_INT_FINITE_STREAM, NTH_EMPTY)
{
    ASSERT_THROW(stream::Stream<int>() | nth(0), std::out_of_range);
}

TEST(FILTERS_INT_FINITE_STREAM, NTH_SINGLE)
{
    ASSERT_EQ(stream::Stream(125) | nth(0), 125);
}

TEST(FILTERS_INT_FINITE_STREAM, NTH_GENERIC_FIRST)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(0), 1);
}

TEST(FILTERS_INT_FINITE_STREAM, NTH_GENERIC_MIDDLE)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(3), 4);
}

TEST(FILTERS_INT_FINITE_STREAM, NTH_GENERIC_LAST)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(6), 7);
}

TEST(FILTERS_INT_FINITE_STREAM, NTH_GENERIC_OUT_OF_RANGE1)
{
    ASSERT_THROW(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(7), std::out_of_range);
}

TEST(FILTERS_INT_FINITE_STREAM, NTH_GENERIC_OUT_OF_RANGE2)
{
    ASSERT_THROW(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(100), std::out_of_range);
}


TEST(FILTERS_INT_FINITE_STREAM, TO_VECTOR_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, TO_VECTOR_SINGLE)
{
    ASSERT_THAT(stream::Stream(200) | to_vector(), testing::ElementsAre(200));
}

TEST(FILTERS_INT_FINITE_STREAM, TO_VECTOR_GENERIC)
{
    ASSERT_THAT(stream::Stream(1, 2, 3, 4, 5) | to_vector(), testing::ElementsAre(1, 2, 3, 4, 5));
}


TEST(FILTERS_INT_FINITE_STREAM, FILTER_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | filter([](auto &&) { return true; }) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, FILTER_SINGLE_TO_EMPTY)
{
    ASSERT_TRUE((stream::Stream(45) | filter([](auto &&) { return false; }) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, FILTER_SINGLE_TO_SINGLE)
{
    ASSERT_THAT(stream::Stream(45) | filter([](auto &&) { return true; }) | to_vector(),
                testing::ElementsAre(45));
}

TEST(FILTERS_INT_FINITE_STREAM, FILTER_GENERIC_TO_EMPTY)
{
    ASSERT_TRUE((stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | filter([](auto &&) { return false; }) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, FILTER_GENERIC_TO_ITSELF)
{
    ASSERT_THAT(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | filter([](auto &&) { return true; }) | to_vector(),
                testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));
}

TEST(FILTERS_INT_FINITE_STREAM, FILTER_GENERIC_ONLY_EVEN)
{
    ASSERT_THAT(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | filter([](auto &&v) { return !(v % 2); }) | to_vector(),
                testing::ElementsAre(2, 4, 6, 8));
}


TEST(FILTERS_INT_FINITE_STREAM, GROUP_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | group(1) | to_vector()).empty());
}

TEST(FILTERS_INT_FINITE_STREAM, GROUP_SINGLE_BY_1)
{
    auto result = stream::Stream(45) | group(1) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(45));
}

TEST(FILTERS_INT_FINITE_STREAM, GROUP_SINGLE_BY_2)
{
    auto result = stream::Stream(45) | group(2) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(45));
}

TEST(FILTERS_INT_FINITE_STREAM, GROUP_GENERIC_BY_1)
{
    auto result = stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | group(1) | to_vector();

    ASSERT_EQ(result.size(), 9U);
    for (int i = 0; i < 9; ++i)
    {
        ASSERT_THAT(result[i], testing::ElementsAre(i + 1));
    }
}

TEST(FILTERS_INT_FINITE_STREAM, GROUP_GENERIC_TO_FEW)
{
    auto result = stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | group(4) | to_vector();

    ASSERT_EQ(result.size(), 3U);
    ASSERT_THAT(result[0], testing::ElementsAre(1, 2, 3, 4));
    ASSERT_THAT(result[1], testing::ElementsAre(5, 6, 7, 8));
    ASSERT_THAT(result[2], testing::ElementsAre(9));
}

TEST(FILTERS_INT_FINITE_STREAM, GROUP_GENERIC_TO_ONE_SAME)
{
    auto result = stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | group(9) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));
}

TEST(FILTERS_INT_FINITE_STREAM, GROUP_GENERIC_TO_ONE_MORE1)
{
    auto result = stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | group(10) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));
}

TEST(FILTERS_INT_FINITE_STREAM, GROUP_GENERIC_TO_ONE_MORE2)
{
    auto result = stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | group(100) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9));
}
*/
