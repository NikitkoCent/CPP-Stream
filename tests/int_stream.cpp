#include <stream.h>
#include <filters_lib.h>
#include <vector>
#include <sstream>
#include <type_traits>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::filters;


TEST(FILTERS_INT_STREAM, PRINT_TO_RETURNVALUE)
{
    std::ostringstream stream;

    ASSERT_TRUE((std::is_same<decltype(stream::Stream{1, 2, 3} | print_to(stream)), decltype(stream)&>::value));
    ASSERT_EQ(&(stream::Stream{1, 2, 3} | print_to(stream)), &stream);
}

TEST(FILTERS_INT_STREAM, PRINT_TO_EMPTY)
{
    std::ostringstream stream;

    stream::Stream<int>() | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, PRINT_TO_1)
{
    std::ostringstream stream;

    stream::Stream(20) | print_to(stream);
    ASSERT_EQ(stream.str(), "20");
}

TEST(FILTERS_INT_STREAM, PRINT_TO_2)
{
    std::ostringstream stream;

    stream::Stream(1, 2) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2");
}

TEST(FILTERS_INT_STREAM, PRINT_TO_GENERIC)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5 6 7 8 9");
}


TEST(FILTERS_INT_STREAM, SKIP_0)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(0) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3");
}

TEST(FILTERS_INT_STREAM, SKIP_1)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(1) | print_to(stream);
    ASSERT_EQ(stream.str(), "2 3");
}

TEST(FILTERS_INT_STREAM, SKIP_ALL)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(3) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, SKIP_ALL_BUT_ONE)
{
    std::ostringstream stream;

    stream::Stream{1, 2, 3} | skip(2) | print_to(stream);
    ASSERT_EQ(stream.str(), "3");
}

TEST(FILTERS_INT_STREAM, SKIP_DOUBLE)
{
    std::ostringstream stream;

    Stream a = Stream(1, 2, 3) | skip(1);
    Stream b = a | skip(1);

    b | print_to(stream);
    ASSERT_EQ(stream.str(), "3");
}


TEST(FILTERS_INT_STREAM, MAP_EMPTY)
{
    std::ostringstream stream;

    stream::Stream<int>() | map([](auto &&v){ return std::move(v); }) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, MAP_1)
{
    std::ostringstream stream;

    stream::Stream(15) | map([](auto &&v){ return v * v; }) | print_to(stream);
    ASSERT_EQ(stream.str(), "225");
}

TEST(FILTERS_INT_STREAM, MAP_GENERIC)
{
    std::ostringstream stream;

    stream::Stream(-1, 2, -3, 4, -5, 6, -7, 8, -9) | map([](auto &&v){ return v * v ;}) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 4 9 16 25 36 49 64 81");
}


TEST(FILTERS_INT_STREAM, GET_0_EMPTY)
{
    std::ostringstream stream;

    stream::Stream<int>() | get(0) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, GET_0_NOTEMPTY)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3) | get(0) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, GET_1_EMPTY)
{
    std::ostringstream stream;

    stream::Stream<int>() | get(1) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, GET_1_NOTEMPTY)
{
    std::ostringstream stream;

    stream::Stream(5, 2, 3) | get(1) | print_to(stream);
    ASSERT_EQ(stream.str(), "5");
}

TEST(FILTERS_INT_STREAM, GET_1_FROM_SINGLE)
{
    std::ostringstream stream;

    stream::Stream(5) | get(1) | print_to(stream);
    ASSERT_EQ(stream.str(), "5");
}

TEST(FILTERS_INT_STREAM, GET_ALL)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3, 4, 5) | get(5) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5");
}

TEST(FILTERS_INT_STREAM, GET_OUT_OF_RANGE)
{
    std::ostringstream stream;

    stream::Stream(1, 2, 3, 4, 5) | get(6) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5");
}


auto sumReducer = [](auto &&v1, auto &&v2) { return v1 + v2; };

TEST(FILTERS_INT_STREAM, REDUCE_EMPTY)
{
    ASSERT_EQ(stream::Stream<int>() | reduce(sumReducer), 0);
}

TEST(FILTERS_INT_STREAM, REDUCE_SINGLE)
{
    ASSERT_EQ(stream::Stream(10) | reduce(sumReducer), 10);
}

TEST(FILTERS_INT_STREAM, REDUCE_DOUBLE)
{
    ASSERT_EQ(stream::Stream(45, -100) | reduce(sumReducer), -55);
}

TEST(FILTERS_INT_STREAM, REDUCE_GENERIC)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) | reduce(sumReducer), 55);
}

auto stringReducer = [](auto &&str, auto &&num) { return std::move(str += " " + std::to_string(num)); };
auto stringInit = [](auto &&num) { return std::to_string(num); };

TEST(FILTERS_INT_STREAM, REDUCE_TO_STRING_EMPTY)
{
    ASSERT_TRUE((stream::Stream<int>() | reduce(stringInit, stringReducer)).empty());
}

TEST(FILTERS_INT_STREAM, REDUCE_TO_STRING_SINGLE)
{
    ASSERT_EQ(stream::Stream(10) | reduce(stringInit, stringReducer), "10");
}

TEST(FILTERS_INT_STREAM, REDUCE_TO_STRING_DOUBLE)
{
    ASSERT_EQ(stream::Stream(45, -100) | reduce(stringInit, stringReducer), "45 -100");
}

TEST(FILTERS_INT_STREAM, REDUCE_TO_STRING_GENERIC)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) | reduce(stringInit, stringReducer),
              "1 2 3 4 5 6 7 8 9 10");
}


TEST(FILTERS_INT_STREAM, SUM_EMPTY)
{
    ASSERT_EQ(stream::Stream<int>() | sum(), 0);
}

TEST(FILTERS_INT_STREAM, SUM_SINGLE)
{
    ASSERT_EQ(stream::Stream(10) | sum(), 10);
}

TEST(FILTERS_INT_STREAM, SUM_DOUBLE)
{
    ASSERT_EQ(stream::Stream(45, -100) | sum(), -55);
}

TEST(FILTERS_INT_STREAM, SUM_GENERIC)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) | sum(), 55);
}


TEST(FILTERS_INT_STREAM, NTH_EMPTY)
{
    ASSERT_THROW(stream::Stream<int>() | nth(0), std::out_of_range);
}

TEST(FILTERS_INT_STREAM, NTH_SINGLE)
{
    ASSERT_EQ(stream::Stream(125) | nth(0), 125);
}

TEST(FILTERS_INT_STREAM, NTH_GENERIC_FIRST)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(0), 1);
}

TEST(FILTERS_INT_STREAM, NTH_GENERIC_MIDDLE)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(3), 4);
}

TEST(FILTERS_INT_STREAM, NTH_GENERIC_LAST)
{
    ASSERT_EQ(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(6), 7);
}

TEST(FILTERS_INT_STREAM, NTH_GENERIC_OUT_OF_RANGE1)
{
    ASSERT_THROW(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(7), std::out_of_range);
}

TEST(FILTERS_INT_STREAM, NTH_GENERIC_OUT_OF_RANGE2)
{
    ASSERT_THROW(stream::Stream(1, 2, 3, 4, 5, 6, 7) | nth(100), std::out_of_range);
}
