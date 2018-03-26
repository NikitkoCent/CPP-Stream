#ifndef CPP_STREAM_DETAIL_STREAM_BASE_H
#define CPP_STREAM_DETAIL_STREAM_BASE_H

#include "traits_impl.h"        // InvokeResultT
#include "stream_traits_impl.h" // IsStreamFilterFor
#include <type_traits>          // ::std::enable_if_t, ::std::decay_t, ::std::is_reference
#include <utility>              // ::std::move, ::std::forward

namespace stream
{
    template<typename, typename>
    struct Stream;

    namespace detail
    {
        template<typename S, typename Filter, typename StreamImpl>
        class StreamFilter;


        template<typename T, bool Finiteness, typename StreamImpl>
        class StreamBase
        {
        public:
            static_assert(!::std::is_reference<T>::value, "Stream of references isn't allowed");


            using Type = T;
            static constexpr bool IsFinite = Finiteness;


            template<typename Filter, typename ::std::enable_if_t<IsStreamFilterFor<Filter, StreamImpl>::value>* = nullptr>
            auto operator|(Filter &&filter)
            {
                using V = typename InvokeResultT<::std::decay_t<Filter>, const Type&, const StreamImpl&>::value_type;
                return stream::Stream<V, StreamFilter<StreamImpl, Filter, void>>(::std::move(static_cast<StreamImpl&>(*this)),
                                                                                 ::std::forward<Filter>(filter));
            }

            template<typename NonFilter, typename ::std::enable_if_t<!IsStreamFilterFor<NonFilter, StreamImpl>::value>* = nullptr>
            decltype(auto) operator|(NonFilter &&nonFilter)
            {
                return ::std::forward<NonFilter>(nonFilter)(::std::move(static_cast<StreamImpl&>(*this)));
            }


            bool isEnd() const
            {
                static_assert(IsFinite, "This stream is infinite");
                return static_cast<const StreamImpl *>(this)->isEndImpl();
            }
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_BASE_H
