#ifndef CPP_STREAM_STREAM_VISITORS_H
#define CPP_STREAM_STREAM_VISITORS_H

#include "detail/utility.h"
#include "detail/stream_visitors.h"
#include "stream.h"     // stream::Stream
#include "visitor.h"    // stream::Visitor
#include <cstddef>      // ::std::size_t
#include <utility>      // ::std::move, ::std::forward
#include <vector>       // ::std::vector
#include <stdexcept>    // ::std::out_of_range
#include <iterator>     // ::std::advance, ::std::back_inserter
#include <type_traits>  // ::std::decay_t
#include <functional>   // ::std::plus
#include <algorithm>    // ::std::copy


namespace stream
{
    namespace visitors
    {
        auto get(::std::size_t n)
        {
            auto lambda = [n](const auto &stream) mutable {
                using StreamValueType = detail::StreamValueT<decltype(stream)>;
                using Type = StreamValueType;

                ::std::vector<Type> result;
                result.reserve(n);

                auto iter = stream.begin();
                while (n > 0)
                {
                    result.emplace_back(*iter++);
                    --n;
                }

                return stream::Stream<Type, ::std::vector<Type>>(::std::move(result));
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }

        template<typename OStream>
        auto print_to(OStream &oStream, const char *delim = " ")
        {
            auto lambda = [&oStream, delim](const auto &stream) mutable -> OStream& {
                auto iter = stream.begin();
                if (iter == stream.end())
                {
                    return oStream;
                }

                oStream << *iter;
                for (++iter; iter != stream.end(); ++iter)
                {
                    oStream << delim << *iter;
                }

                return oStream;
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }


        template<typename Callable>
        auto map(Callable &&callable)
        {
            auto lambda = [f = ::std::forward<Callable>(callable)](const auto &stream) mutable {
                using StreamValueType = detail::StreamValueT<decltype(stream)>;
                using Type = ::std::decay_t<detail::InvokeResultT<decltype(f), StreamValueType>>;

                ::std::vector<Type> result;
                for (const StreamValueType &value : stream)
                {
                    result.emplace_back(f(value));
                }

                return Stream<Type, ::std::vector<Type>>(::std::move(result));
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }


        template<typename Accumulator, typename Identity>
        auto reduce(Identity &&identity, Accumulator &&accumulator)
        {
            auto lambda = [accumulator = ::std::forward<Accumulator>(accumulator),
                           identity = ::std::forward<Identity>(identity)](const auto &stream) mutable {
                using StreamValueType = detail::StreamValueT<decltype(stream)>;
                using Type = detail::InvokeResultT<decltype(identity), const StreamValueType&>;

                if (stream.begin() == stream.end())
                {
                    return Type{};
                }
                else
                {
                    auto iter = stream.begin();

                    Type result = identity(*iter);
                    while (++iter != stream.end())
                    {
                        result = accumulator(result, *iter);
                    }
                    return result;
                }
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }

        template<typename Accumulator>
        auto reduce(Accumulator &&accumulator)
        {
            return reduce([](const auto &v) { return v; }, ::std::forward<Accumulator>(accumulator));
        }


        auto to_vector()
        {
            auto lambda = [](const auto &stream) {
                using StreamValueType = detail::StreamValueT<decltype(stream)>;

                ::std::vector<StreamValueType> result;
                for (const auto &v : stream)
                {
                    result.emplace_back(v);
                }

                return result;
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }


        auto nth(::std::size_t index)
        {
            auto lambda = [index](const auto &stream) mutable {
                auto iter = stream.begin();

                if constexpr (detail::StreamFinitenessV<decltype(stream)>)
                {
                    while (index > 0)
                    {
                        if (iter == stream.end())
                        {
                            throw ::std::out_of_range{"Stream out of range"};
                        }

                        ++iter;
                        --index;
                    }
                }
                else
                {
                    ::std::advance(iter, index);
                }

                return *iter;
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }


        auto sum()
        {
            return reduce(::std::plus<>{});
        }


        auto skip(::std::size_t amount)
        {
            auto lambda = [amount](const auto &stream) mutable {
                using StreamValueType = detail::StreamValueT<decltype(stream)>;

                auto iter = stream.begin();
                if constexpr (detail::StreamFinitenessV<decltype(stream)>)
                {
                    ::std::vector<StreamValueType> result;
                    while (amount > 0)
                    {
                        if (iter == stream.end())
                        {
                            throw ::std::out_of_range{"Stream out of range"};
                        }

                        ++iter;
                        --amount;
                    }

                    result.insert(result.cend(), iter, stream.end());
                    return stream::Stream<StreamValueType, ::std::vector<StreamValueType>>(::std::move(result));
                }
                else
                {
                    ::std::advance(iter, amount);
                    return typename detail::StreamTraits<decltype(stream)>::StreamType{stream};
                }
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }


        template<typename Filter>
        auto filter(Filter &&f)
        {
            auto lambda = [f = ::std::forward<Filter>(f)](const auto &stream) {
                using StreamValueType = detail::StreamValueT<decltype(stream)>;

                ::std::vector<StreamValueType> result;
                ::std::copy_if(stream.begin(), stream.end(), ::std::back_inserter(result), ::std::move(f));

                return stream::Stream<StreamValueType, ::std::vector<StreamValueType>>(::std::move(result));
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }
    }
}

#endif //CPP_STREAM_STREAM_VISITORS_H
