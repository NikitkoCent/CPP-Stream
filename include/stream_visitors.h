#ifndef CPP_STREAM_STREAM_VISITORS_H
#define CPP_STREAM_STREAM_VISITORS_H

#include "stream_traits.h"              // StreamValueT
#include "traits.h"                     // detail::InvokeResultT
#include "filter.h"                     // makeFilter
#include <cstddef>                      // ::std::size_t
#include <optional>                     // ::std::optional
#include <utility>                      // ::std::move, ::std::forward


namespace stream
{
    namespace visitors
    {
        template<typename OStream>
        auto print_to(OStream &oStream, const char *delim = " ")
        {
            return [&oStream, delim](auto &&stream) -> OStream& {
                while (!stream.isEnd())
                {
                    auto val = stream.getNext();
                    if (val)
                    {
                        oStream << val.value() << delim;
                    }
                }

                return oStream;
            };
        }


        auto skip(::std::size_t amount)
        {
            return makeFilter<false>([amount](auto &&value, auto &&stream, bool&) mutable {
                using Type = stream::StreamValueT<decltype(stream)>;
                //using Type = int;

                if (amount > 0) {
                    --amount;
                    return ::std::optional<Type>{::std::nullopt};
                }

                return ::std::optional<Type>{::std::forward<decltype(value)>(value)};
            });
        }


        template<typename Transform>
        auto map(Transform &&transform)
        {
            return makeFilter<false>([transform = ::std::forward<Transform>(transform)](auto &&value, auto&&, bool&) mutable {
                using Type = detail::InvokeResultT<::std::decay_t<Transform>, decltype(value)>;
                return ::std::optional<Type>{transform(::std::forward<decltype(value)>(value))};
            });
        }


        auto get(::std::size_t n)
        {
            return makeFilter<true>([n](auto &&value, auto &&stream, bool &end) mutable {
                using Type = stream::StreamValueT<decltype(stream)>;
                if (n > 0)
                {
                    --n;
                    return ::std::optional<Type>{::std::forward<decltype(value)>(value)};
                }

                end = true;
                return ::std::optional<Type>{::std::nullopt};
            });
        }
    }
}

#endif //CPP_STREAM_STREAM_VISITORS_H
