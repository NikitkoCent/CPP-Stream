#include <streamv2.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <list>
#include <initializer_list>
#include <type_traits>
#include <string>


TEST(DEDUCTION_GUIDES_CONTAINER, RREF)
{
    std::list<std::string> c{"one", "two", "three"};

    stream::Stream stream(std::move(c));

    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<std::string, std::list<std::string>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, CONST_RREF)
{
    const std::list<std::string> c{"one", "two", "three"};

    stream::Stream stream(std::move(c));

    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<std::string, const std::list<std::string>&>>::value));
}



TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST)
{
    stream::Stream stream({1, 2, 3});
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_RREF)
{
    auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), std::initializer_list<int>>::value));
    stream::Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_RREF)
{
    const auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), const std::initializer_list<int>>::value));
    stream::Stream stream(std::move(l));
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_LREF)
{
    auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), std::initializer_list<int>>::value));
    stream::Stream stream(l);
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}

TEST(DEDUCTION_GUIDES_CONTAINER, INITIALIZER_LIST_CONST_LREF)
{
    const auto l = {1, 2, 3};
    ASSERT_TRUE((std::is_same<decltype(l), const std::initializer_list<int>>::value));
    stream::Stream stream(l);
    ASSERT_TRUE((std::is_same<decltype(stream), stream::Stream<int, std::vector<int>>>::value));
}
