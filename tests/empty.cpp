#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <list>
#include <initializer_list>
#include <iostream>
#include <stream.h>

using namespace stream::detail;

TEST(A, B)
{
    auto lambda = [] { std::cout << "12312" << std::endl; };
    AutoInvokableFunctor<decltype(lambda)>(std::move(lambda))
        .then([] { std::cout << "sdfsdgh" << std::endl; });
}
