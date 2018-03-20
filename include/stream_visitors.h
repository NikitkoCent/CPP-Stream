#ifndef CPP_STREAM_STREAM_VISITORS_H
#define CPP_STREAM_STREAM_VISITORS_H

#include "stream.h"
#include <cstddef>      // ::std::size_t
#include <functional>   // ::std::ref

namespace stream
{
    namespace visitors
    {
        /*auto get(::std::size_t n)
        {
            return [n](auto &stream) mutable {
                using type = typename ::std::remove_reference_t<decltype(stream)>::value_type;

                ::std::vector<type> result;
                result.reserve(n);

                auto iter = stream.begin();
                while (n > 0)
                {
                    result.emplace_back(*iter++);
                    --n;
                }

                return Stream<type>(::std::move(result));
            };
        }

        template<typename OStream>
        auto printTo(OStream &oStream, const char *delim = " ")
        {
            return [&oStream, delim](auto &stream) mutable -> OStream& {
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
        }*/
    }
}

#endif //CPP_STREAM_STREAM_VISITORS_H
