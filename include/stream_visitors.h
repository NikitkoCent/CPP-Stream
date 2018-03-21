#ifndef CPP_STREAM_STREAM_VISITORS_H
#define CPP_STREAM_STREAM_VISITORS_H

#include "detail/utility.h"
#include "stream.h"
#include "visitor.h"    // stream::Visitor
#include <cstddef>      // ::std::size_t
#include <type_traits>  // ::std::remove_reference_t
#include <utility>      // ::std::move, ::std::forward
#include <functional>
#include <vector>


namespace stream
{
    namespace visitors
    {
        auto get(::std::size_t n)
        {
            auto lambda = [n](auto &stream) mutable {
                using StreamValueType = typename ::std::remove_reference_t<decltype(stream)>::value_type;
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
            auto lambda = [oStream = ::std::ref(oStream), delim](auto &stream) mutable -> OStream& {
                auto iter = stream.begin();
                if (iter == stream.end())
                {
                    return oStream.get();
                }

                oStream.get() << *iter;
                for (++iter; iter != stream.end(); ++iter)
                {
                    oStream.get() << delim << *iter;
                }

                return oStream.get();
            };

            return stream::Visitor<decltype(lambda)>(::std::move(lambda));
        }


        template<typename Callable>
        auto map(Callable &&callable)
        {
            auto lambda = [f = ::std::forward<Callable>(callable)](auto &stream) mutable {
                using StreamValueType = typename ::std::remove_reference_t<decltype(stream)>::value_type;
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
    }
}

#endif //CPP_STREAM_STREAM_VISITORS_H
