#include "noisy.h"
#include <stream.h>
#include <filters_lib.h>
#include <vector>
#include <sstream>
#include <type_traits>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace stream;
using namespace stream::filters;


TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_EMPTY)
{
    std::ostringstream stream;

    stream::Stream<Noisy>() | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_1)
{
    std::ostringstream stream;

    stream::Stream(Noisy(20)) | print_to(stream);
    ASSERT_EQ(stream.str(), "20");
}

TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_2)
{
    std::ostringstream stream;

    stream::Stream(Noisy(1), Noisy(2)) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2");
}

TEST(FILTERS_NOISY_FINITE_STREAM, PRINT_TO_GENERIC)
{
    std::ostringstream stream;

    stream::Stream(Noisy(1), Noisy(2), Noisy(3), Noisy(4), Noisy(5), Noisy(6), Noisy(7), Noisy(8),
                   Noisy(9)) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2 3 4 5 6 7 8 9");
}


TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_0)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(0) | to_vector();
    ASSERT_EQ(result.size(), 3U);
    ASSERT_EQ(result[0].copyCount, 0U);
    ASSERT_EQ(result[1].copyCount, 0U);
    ASSERT_EQ(result[2].copyCount, 0U);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_1)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(1) | to_vector();
    ASSERT_EQ(result.size(), 2U);
    ASSERT_EQ(result[0].copyCount, 0U);
    ASSERT_EQ(result[1].copyCount, 0U);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_ALL)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(3) | to_vector();
    ASSERT_TRUE(result.empty());
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_ALL_BUT_ONE)
{
    auto result = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(2) | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0U);
}

TEST(FILTERS_NOISY_FINITE_STREAM, SKIP_DOUBLE)
{
    Stream a = Stream(Noisy(1), Noisy(2), Noisy(3)) | skip(1);
    Stream b = a | skip(1);

    auto result = b | to_vector();
    ASSERT_EQ(result.size(), 1U);
    ASSERT_EQ(result[0].copyCount, 0U);
}
