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

    std::vector<int> v;
    stream::Stream(v) | print_to(stream);
    ASSERT_TRUE(stream.str().empty());
}

TEST(FILTERS_INT_STREAM, PRINT_TO_1)
{
    std::ostringstream stream;

    std::vector<int> v;
    stream::Stream(20) | print_to(stream);
    ASSERT_EQ(stream.str(), "20");
}

TEST(FILTERS_INT_STREAM, PRINT_TO_2)
{
    std::ostringstream stream;

    std::vector<int> v;
    stream::Stream(1, 2) | print_to(stream);
    ASSERT_EQ(stream.str(), "1 2");
}

TEST(FILTERS_INT_STREAM, PRINT_TO_GENERIC)
{
    std::ostringstream stream;

    std::vector<int> v;
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
