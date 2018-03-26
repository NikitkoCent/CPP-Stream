#ifndef CPP_STREAM_DETAIL_STREAM_FILTER_H
#define CPP_STREAM_DETAIL_STREAM_FILTER_H

#include "stream_base.h"        // StreamBase
#include "traits_impl.h"        // InvokeResultT
#include "stream_traits_impl.h" // StreamValueT
#include <type_traits>          // ::std::decay_t, ::std::enable_if_t, ::std::is_same
#include <utility>              // ::std::move, ::std::forward
#include <optional>             // ::std::optional

namespace stream
{
    namespace detail
    {
        #define SFPARENT StreamBase<typename InvokeResultT<::std::decay_t<Filter>, \
                                                           const StreamValueT<S>&, const S&>::value_type, \
                                    StreamFinitenessV<S>, StreamImpl>


        template<typename S, typename Filter, typename StreamImpl = void>
        class StreamFilter : public SFPARENT
        {
        public:
            template<typename Callable>
            StreamFilter(S &&stream, Callable &&filter)
                : stream(::std::move(stream)), filter(::std::forward<Callable>(filter))
            {}


            ::std::optional<typename SFPARENT::Type> getNext()
            {
                auto fromStream = stream.getNext();
                if (!fromStream)
                {
                    return { ::std::nullopt };
                }

                return filter(static_cast<const StreamValueT<S>&>(fromStream.value()), static_cast<const S&>(stream));
            }

        private:
            friend class SFPARENT;


            S stream;
            ::std::decay_t<Filter> filter;


            template<bool Fin = SFPARENT::IsFinite>
            ::std::enable_if_t<Fin, bool> isEndImpl() const
            {
                return stream.isEnd();
            };
        };

        template<typename S, typename Filter>
        class StreamFilter<S, Filter, void>
        {};


        #undef SFPARENT
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_FILTER_H
