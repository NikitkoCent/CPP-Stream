#include <stream/stream.h>
#include <stream/operations.h>
#include <list>
#include <vector>
#include <string>
#include <numeric>
#include <functional>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using stream::Stream;
using namespace stream::ops;


TEST(FILTERS_GENERIC_MOVE_ONLY_STREAM, CONTAINER)
{
    std::list<std::unique_ptr<int>> list;

    for (int i = 1; i < 11; ++i)
    {
        list.emplace_back(std::make_unique<int>(i));
    }

    ASSERT_THAT(Stream(std::move(list))
                | filter([](auto &&v){ return *v > 2; })
                | map([](auto &&v){ *v = *v * *v; return std::move(v); })
                | map([](auto &&v){ *v = *v + 1; return std::move(v); })
                | filter([](auto &&v) { return *v > 25; })
                | to_vector(),
                testing::ElementsAre(testing::Pointee(26),
                                     testing::Pointee(37),
                                     testing::Pointee(50),
                                     testing::Pointee(65),
                                     testing::Pointee(82),
                                     testing::Pointee(101)));

    ASSERT_THAT((stream::Stream(std::move(list))
                 | filter([](auto &&v){ return *v > 2; })
                 | map([](auto &&v){ *v = *v * *v; return std::move(v); })
                 | map([](auto &&v){ *v = *v + 1; return std::move(v); })
                 | filter([](auto &&v) { return *v > 25; })
                 | reduce([](auto &&v1, auto &&v2) { *v1 = *v1 + *v2; return std::move(v1); })).get(),
                nullptr);

    ASSERT_THAT(Stream(std::move(list))
                | filter([](auto &&v){ return *v > 2; })
                | map([](auto &&v){ *v = *v * *v; return std::move(v); })
                | map([](auto &&v){ *v = *v + 1; return std::move(v); })
                | filter([](auto &&v) { return *v > 25; })
                | to_vector(), testing::ElementsAre());
}

TEST(FILTERS_GENERIC_MOVE_ONLY_STREAM, GENERATOR)
{
    ASSERT_THAT(Stream([i = int{}]() mutable { return std::make_unique<int>(++i); })
                | skip(20)
                | filter([](auto &&v){ return ((*v % 2) == 1); })
                | get(20)
                | reduce([](auto &&v){ return std::to_string(*v); },
                         [](auto &&str, auto &&v){ return std::move(str += " " + std::to_string(*v)); }),
                "21 23 25 27 29 31 33 35 37 39 41 43 45 47 49 51 53 55 57 59");
}

#if !defined(__GNUG__) || defined(__clang__)
TEST(FILTERS_GENERIC_MOVE_ONLY_STREAM, STREAM_FROM_STREAM)
{
    std::ostringstream out;

    Stream a = Stream([i = int{}]() mutable { return std::make_unique<int>(++i); })
               | group(5)
               | map([](auto &&vec){
                   if (!vec.empty()) {
                       for (unsigned i = 1; i < vec.size(); ++i)
                       {
                            *vec[0] += *vec[i];
                       }

                       return std::move(vec[0]);
                   }

                    return std::unique_ptr<int>{};
               });

    Stream b = a | filter([](auto &&v){ return (*v % 4 != 0); });
    Stream c = b | map([](auto &&v){ return std::to_string(*v); });
    c | get(10) | print_to(out);

    ASSERT_EQ(out.str(), "15 65 90 115 165 190 215 265 290 315");
}
#endif // #if !defined(__GNUG__) || defined(__clang__)
