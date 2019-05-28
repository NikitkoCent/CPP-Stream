#include <stream/stream.h>
#include <stream/operations.h>
#include <vector>
#include <sstream>
#include <type_traits>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::ops;


TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, SKIP_0)
{
    auto result = Stream(std::make_unique<int>(20),
                         std::make_unique<int>(21),
                         std::make_unique<int>(22)) | skip(0) | to_vector();

    ASSERT_EQ(result.size(), 3U);
    ASSERT_EQ(*result[0], 20);
    ASSERT_EQ(*result[1], 21);
    ASSERT_EQ(*result[2], 22);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, SKIP_1)
{
    auto result = Stream(std::make_unique<int>(20),
                         std::make_unique<int>(21),
                         std::make_unique<int>(22)) | skip(1) | to_vector();

    ASSERT_EQ(result.size(), 2U);
    ASSERT_EQ(*result[0], 21);
    ASSERT_EQ(*result[1], 22);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, SKIP_ALL)
{
    ASSERT_TRUE((stream::Stream(std::make_unique<int>(1),
                                std::make_unique<int>(2),
                                std::make_unique<int>(3))
                 | skip(3)
                 | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, SKIP_ALL_BUT_ONE)
{
    auto result = Stream(std::make_unique<int>(20),
                         std::make_unique<int>(21),
                         std::make_unique<int>(22)) | skip(2) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(*result[0], 22);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, SKIP_DOUBLE)
{
    Stream a = Stream(std::make_unique<int>(20),
                      std::make_unique<int>(21),
                      std::make_unique<int>(22)) | skip(1);
    Stream b = a | skip(1);

    auto result = b | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(*result[0], 22);
}


TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, MAP_EMPTY)
{
    ASSERT_TRUE((Stream<std::unique_ptr<int>>()
                 | map([](auto &&v){ return std::move(v); })
                 | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, MAP_1)
{
    Stream myStream(std::make_unique<int>(15));
    auto vec = myStream
               | map([](auto &&v){ *v = *v * *v; return std::move(v); })
               | to_vector();

    ASSERT_EQ(vec.size(), 1U);
    ASSERT_EQ(*vec[0], 225);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, MAP_GENERIC)
{
    auto vec = Stream(std::make_unique<int>(-1),
                      std::make_unique<int>(2),
                      std::make_unique<int>(-3),
                      std::make_unique<int>(4),
                      std::make_unique<int>(-5),
                      std::make_unique<int>(6),
                      std::make_unique<int>(-7),
                      std::make_unique<int>(8),
                      std::make_unique<int>(-9))
               | map([](auto &&v){ *v =  *v * *v ; return std::move(v); })
               | to_vector();

    ASSERT_EQ(vec.size(), 9U);
    for (int i = 0; i < 9; ++i)
    {
        ASSERT_EQ(*vec[i], (i + 1) * (i + 1));
    }
}


TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GET_0_EMPTY)
{
    ASSERT_TRUE((Stream<std::unique_ptr<int>>() | get(0) | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GET_0_NOTEMPTY)
{
    ASSERT_TRUE((stream::Stream(std::make_unique<int>(1),
                                std::make_unique<int>(2),
                                std::make_unique<int>(3)) | get(0) | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GET_1_EMPTY)
{
    ASSERT_TRUE((Stream<std::unique_ptr<int>>() | get(1) | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GET_1_NOTEMPTY)
{
    auto vec = Stream(std::make_unique<int>(5),
                      std::make_unique<int>(2),
                      std::make_unique<int>(3)) | get(1) | to_vector();

    ASSERT_EQ(vec.size(), 1U);
    ASSERT_EQ(*vec[0], 5);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GET_1_FROM_SINGLE)
{
    auto vec = Stream(std::make_unique<int>(5))
               | get(1)
               | to_vector();

    ASSERT_EQ(vec.size(), 1U);
    ASSERT_EQ(*vec[0], 5);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GET_ALL)
{
    auto vec = Stream(std::make_unique<int>(1),
                      std::make_unique<int>(2),
                      std::make_unique<int>(3),
                      std::make_unique<int>(4),
                      std::make_unique<int>(5)) | get(5) | to_vector();

    ASSERT_EQ(vec.size(), 5U);
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_EQ(*vec[i], i + 1);
    }
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GET_OUT_OF_RANGE)
{
    auto vec = Stream(std::make_unique<int>(1),
                      std::make_unique<int>(2),
                      std::make_unique<int>(3),
                      std::make_unique<int>(4),
                      std::make_unique<int>(5)) | get(6) | to_vector();

    ASSERT_EQ(vec.size(), 5U);
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_EQ(*vec[i], i + 1);
    }
}


namespace
{
   auto sumReducer = [](auto&& v1, auto&& v2) { *v1 = *v1 + *v2; return std::move(v1); };
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_EMPTY)
{
    ASSERT_EQ(Stream<std::unique_ptr<int>>() | reduce(sumReducer), nullptr);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_SINGLE)
{
    ASSERT_THAT(Stream(std::make_unique<int>(10)) | reduce(sumReducer), testing::Pointee(10));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_DOUBLE)
{
    ASSERT_THAT(Stream(std::make_unique<int>(45),
                       std::make_unique<int>(-100))
                | reduce(sumReducer), testing::Pointee(-55));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_GENERIC)
{
    ASSERT_THAT(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5),
                       std::make_unique<int>(6),
                       std::make_unique<int>(7),
                       std::make_unique<int>(8),
                       std::make_unique<int>(9),
                       std::make_unique<int>(10))
                | reduce(sumReducer), testing::Pointee(55));
}

namespace
{
   auto stringReducer = [](auto&& str, auto&& num) { return std::move(str += " " + std::to_string(*num)); };
   auto stringInit = [](auto&& num) { return std::to_string(*num); };
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_TO_STRING_EMPTY)
{
    ASSERT_TRUE((Stream<std::unique_ptr<int>>() | reduce(stringInit, stringReducer)).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_TO_STRING_SINGLE)
{
    ASSERT_EQ(Stream(std::make_unique<int>(10)) | reduce(stringInit, stringReducer), "10");
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_TO_STRING_DOUBLE)
{
    ASSERT_EQ(Stream(std::make_unique<int>(45),
                     std::make_unique<int>(-100))
              | reduce(stringInit, stringReducer), "45 -100");
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, REDUCE_TO_STRING_GENERIC)
{
    ASSERT_EQ(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5),
                       std::make_unique<int>(6),
                       std::make_unique<int>(7),
                       std::make_unique<int>(8),
                       std::make_unique<int>(9),
                       std::make_unique<int>(10))
              | reduce(stringInit, stringReducer), "1 2 3 4 5 6 7 8 9 10");
}


TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, NTH_EMPTY)
{
    ASSERT_THROW(Stream<std::unique_ptr<int>>() | nth(0), std::out_of_range);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, NTH_SINGLE)
{
    ASSERT_THAT(Stream(std::make_unique<int>(125)) | nth(0), testing::Pointee(125));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, NTH_GENERIC_FIRST)
{
    ASSERT_THAT(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5),
                       std::make_unique<int>(6),
                       std::make_unique<int>(7)) | nth(0), testing::Pointee(1));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, NTH_GENERIC_MIDDLE)
{
    ASSERT_THAT(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5),
                       std::make_unique<int>(6),
                       std::make_unique<int>(7)) | nth(3), testing::Pointee(4));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, NTH_GENERIC_LAST)
{
    ASSERT_THAT(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5),
                       std::make_unique<int>(6),
                       std::make_unique<int>(7)) | nth(6), testing::Pointee(7));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, NTH_GENERIC_OUT_OF_RANGE1)
{
    ASSERT_THROW(stream::Stream(std::make_unique<int>(1),
                                std::make_unique<int>(2),
                                std::make_unique<int>(3),
                                std::make_unique<int>(4),
                                std::make_unique<int>(5),
                                std::make_unique<int>(6),
                                std::make_unique<int>(7)) | nth(7), std::out_of_range);
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, NTH_GENERIC_OUT_OF_RANGE2)
{
    ASSERT_THROW(stream::Stream(std::make_unique<int>(1),
                                std::make_unique<int>(2),
                                std::make_unique<int>(3),
                                std::make_unique<int>(4),
                                std::make_unique<int>(5),
                                std::make_unique<int>(6),
                                std::make_unique<int>(7)) | nth(100), std::out_of_range);
}


TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, TO_VECTOR_EMPTY)
{
    ASSERT_TRUE((Stream<std::unique_ptr<int>>() | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, TO_VECTOR_SINGLE)
{
    ASSERT_THAT(Stream(std::make_unique<int>(200)) | to_vector(),
                testing::ElementsAre(testing::Pointee(200)));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, TO_VECTOR_GENERIC)
{
    ASSERT_THAT(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5)) | to_vector(),
                testing::ElementsAre(testing::Pointee(1), testing::Pointee(2), testing::Pointee(3),
                                     testing::Pointee(4), testing::Pointee(5)));
}


TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, FILTER_EMPTY)
{
    ASSERT_TRUE((stream::Stream<std::unique_ptr<int>>() | filter([](auto &&) { return true; }) | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, FILTER_SINGLE_TO_EMPTY)
{
    ASSERT_TRUE((stream::Stream(std::make_unique<int>(45))
                 | filter([](auto &&) { return false; }) | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, FILTER_SINGLE_TO_SINGLE)
{
    ASSERT_THAT(Stream(std::make_unique<int>(45))
                | filter([](auto &&) { return true; }) | to_vector(),
                testing::ElementsAre(testing::Pointee(45)));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, FILTER_GENERIC_TO_EMPTY)
{
    ASSERT_TRUE((stream::Stream(std::make_unique<int>(1),
                                std::make_unique<int>(2),
                                std::make_unique<int>(3),
                                std::make_unique<int>(4),
                                std::make_unique<int>(5),
                                std::make_unique<int>(6),
                                std::make_unique<int>(7),
                                std::make_unique<int>(8),
                                std::make_unique<int>(9))
                 | filter([](auto &&) { return false; })
                 | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, FILTER_GENERIC_TO_ITSELF)
{
    ASSERT_THAT(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5),
                       std::make_unique<int>(6),
                       std::make_unique<int>(7),
                       std::make_unique<int>(8),
                       std::make_unique<int>(9))
                | filter([](auto &&) { return true; })
                | to_vector(),
                testing::ElementsAre(testing::Pointee(1),
                                     testing::Pointee(2),
                                     testing::Pointee(3),
                                     testing::Pointee(4),
                                     testing::Pointee(5),
                                     testing::Pointee(6),
                                     testing::Pointee(7),
                                     testing::Pointee(8),
                                     testing::Pointee(9)));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, FILTER_GENERIC_ONLY_EVEN)
{
    ASSERT_THAT(Stream(std::make_unique<int>(1),
                       std::make_unique<int>(2),
                       std::make_unique<int>(3),
                       std::make_unique<int>(4),
                       std::make_unique<int>(5),
                       std::make_unique<int>(6),
                       std::make_unique<int>(7),
                       std::make_unique<int>(8),
                       std::make_unique<int>(9))
                | filter([](auto &&v) { return (*v % 2 == 0); })
                | to_vector(),
                testing::ElementsAre(testing::Pointee(2),
                                     testing::Pointee(4),
                                     testing::Pointee(6),
                                     testing::Pointee(8)));
}


// all the next tests will cause compile-time error with gcc 7 (see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80654)
#if !defined(__GNUG__) || defined(__clang__)
TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_EMPTY)
{
    ASSERT_TRUE((stream::Stream<std::unique_ptr<int>>() | group(1) | to_vector()).empty());
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_SINGLE_BY_1)
{
    ASSERT_THAT(Stream(std::make_unique<int>(45)) | group(1) | to_vector(),
                testing::ElementsAre(testing::ElementsAre(testing::Pointee(45))));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_SINGLE_BY_2)
{
    ASSERT_THAT(Stream(std::make_unique<int>(45)) | group(2) | to_vector(),
                testing::ElementsAre(testing::ElementsAre(testing::Pointee(45))));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_GENERIC_BY_1)
{
    auto result = Stream(std::make_unique<int>(1),
                         std::make_unique<int>(2),
                         std::make_unique<int>(3),
                         std::make_unique<int>(4),
                         std::make_unique<int>(5),
                         std::make_unique<int>(6),
                         std::make_unique<int>(7),
                         std::make_unique<int>(8),
                         std::make_unique<int>(9)) | group(1) | to_vector();

    ASSERT_EQ(result.size(), 9U);
    for (int i = 0; i < 9; ++i)
    {
        ASSERT_THAT(result[i], testing::ElementsAre(testing::Pointee(i + 1)));
    }
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_GENERIC_TO_FEW)
{
    auto result = Stream(std::make_unique<int>(1),
                         std::make_unique<int>(2),
                         std::make_unique<int>(3),
                         std::make_unique<int>(4),
                         std::make_unique<int>(5),
                         std::make_unique<int>(6),
                         std::make_unique<int>(7),
                         std::make_unique<int>(8),
                         std::make_unique<int>(9)) | group(4) | to_vector();

    ASSERT_EQ(result.size(), 3U);
    ASSERT_THAT(result[0], testing::ElementsAre(testing::Pointee(1),
                                                testing::Pointee(2),
                                                testing::Pointee(3),
                                                testing::Pointee(4)));
    ASSERT_THAT(result[1], testing::ElementsAre(testing::Pointee(5),
                                                testing::Pointee(6),
                                                testing::Pointee(7),
                                                testing::Pointee(8)));
    ASSERT_THAT(result[2], testing::ElementsAre(testing::Pointee(9)));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_GENERIC_TO_ONE_SAME)
{
    auto result = Stream(std::make_unique<int>(1),
                         std::make_unique<int>(2),
                         std::make_unique<int>(3),
                         std::make_unique<int>(4),
                         std::make_unique<int>(5),
                         std::make_unique<int>(6),
                         std::make_unique<int>(7),
                         std::make_unique<int>(8),
                         std::make_unique<int>(9)) | group(9) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(testing::Pointee(1),
                                                testing::Pointee(2),
                                                testing::Pointee(3),
                                                testing::Pointee(4),
                                                testing::Pointee(5),
                                                testing::Pointee(6),
                                                testing::Pointee(7),
                                                testing::Pointee(8),
                                                testing::Pointee(9)));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_GENERIC_TO_ONE_MORE1)
{
    auto result = Stream(std::make_unique<int>(1),
                         std::make_unique<int>(2),
                         std::make_unique<int>(3),
                         std::make_unique<int>(4),
                         std::make_unique<int>(5),
                         std::make_unique<int>(6),
                         std::make_unique<int>(7),
                         std::make_unique<int>(8),
                         std::make_unique<int>(9)) | group(10) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(testing::Pointee(1),
                                                testing::Pointee(2),
                                                testing::Pointee(3),
                                                testing::Pointee(4),
                                                testing::Pointee(5),
                                                testing::Pointee(6),
                                                testing::Pointee(7),
                                                testing::Pointee(8),
                                                testing::Pointee(9)));
}

TEST(FILTERS_MOVE_ONLY_FINITE_STREAM, GROUP_GENERIC_TO_ONE_MORE2)
{
    auto result = Stream(std::make_unique<int>(1),
                         std::make_unique<int>(2),
                         std::make_unique<int>(3),
                         std::make_unique<int>(4),
                         std::make_unique<int>(5),
                         std::make_unique<int>(6),
                         std::make_unique<int>(7),
                         std::make_unique<int>(8),
                         std::make_unique<int>(9)) | group(100) | to_vector();

    ASSERT_EQ(result.size(), 1U);
    ASSERT_THAT(result[0], testing::ElementsAre(testing::Pointee(1),
                                                testing::Pointee(2),
                                                testing::Pointee(3),
                                                testing::Pointee(4),
                                                testing::Pointee(5),
                                                testing::Pointee(6),
                                                testing::Pointee(7),
                                                testing::Pointee(8),
                                                testing::Pointee(9)));
}

#endif // #if !defined(__GNUG__) || defined(__clang__)
