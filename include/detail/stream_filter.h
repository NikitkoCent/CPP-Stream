#ifndef CPP_STREAM_DETAIL_STREAM_FILTER_H
#define CPP_STREAM_DETAIL_STREAM_FILTER_H

#include "utility.h"
#include "stream_traits.h"
#include <type_traits>      // ::std::decay_t
#include <utility>          // ::std::move
#include <optional>
#include <iterator>

namespace stream
{
    namespace detail
    {
        template<typename Stream, typename Filter>
        class StreamFilter
        {
        public:
            using Type = InvokeResultT<::std::decay_t<Filter>, const StreamValueT<Stream>&>;


            StreamFilter(Stream &&stream, ::std::decay_t<Filter> &&filter)
                : stream(::std::move(stream)), filter(::std::move(filter))
            {
            }


            Type getNext()
            {
                auto next = stream.getNext();
                if (next)
                {
                    return filter(::std::move(next));
                }
                else
                {
                    return { ::std::nullopt };
                }
            }

            bool isEnd() const
            {
                return stream.isEnd();
            }

        private:
            typename StreamTraits<Stream>::StreamType stream;
            ::std::decay_t<Filter> filter;
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_FILTER_H
