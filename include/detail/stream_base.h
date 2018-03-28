#ifndef CPP_STREAM_DETAIL_STREAM_BASE_H
#define CPP_STREAM_DETAIL_STREAM_BASE_H

#include "traits_impl.h"                    // InvokeResultT
#include "stream_traits_impl.h"             // IsStreamFilterFor
#include "is_filters_factory_for_impl.h"    // IsFiltersFactoryFor
#include <type_traits>                      // ::std::enable_if_t, ::std::decay_t, ::std::is_reference
#include <utility>                          // ::std::move, ::std::forward

namespace stream
{
    template<typename, typename>
    struct Stream;

    namespace detail
    {
        template<typename, typename, typename>
        class StreamFilter;


        template<typename T, bool Finiteness, typename StreamImpl>
        class StreamBase
        {
        public:
            using Type = ::std::conditional_t<::std::is_reference<T>::value,
                                              ::std::reference_wrapper<::std::remove_reference_t<T>>,
                                              T>;
            static constexpr bool IsFinite = Finiteness;


            template<typename Filter, typename ::std::enable_if_t<IsStreamFilterFor<Filter, StreamImpl>::value>* = nullptr>
            auto operator|(Filter &&filter)
            {
                using V = typename InvokeResultT<::std::decay_t<Filter>, Type&&, const StreamImpl&, bool&>::value_type;
                return stream::Stream<V, StreamFilter<StreamImpl, Filter, void>>(::std::move(static_cast<StreamImpl&>(*this)),
                                                                                 ::std::forward<Filter>(filter));
            }


            template<typename Factory, typename ::std::enable_if_t<!IsStreamFilterFor<Factory, StreamImpl>::value
                                                                   && IsFiltersFactoryFor<Factory, StreamImpl>::value>* = nullptr>
            auto operator|(Factory &&factory)
            {
                return operator|(::std::forward<Factory>(factory).template createFilter<StreamImpl>());
            };


            template<typename Term, typename ::std::enable_if_t<!IsStreamFilterFor<Term, StreamImpl>::value
                                                                && !IsFiltersFactoryFor<Term, StreamImpl>::value>* = nullptr>
            decltype(auto) operator|(Term &&nonFilter)
            {
                return ::std::forward<Term>(nonFilter)(::std::move(static_cast<StreamImpl&>(*this)));
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
