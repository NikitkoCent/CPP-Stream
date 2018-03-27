#ifndef CPP_STREAM_DETAIL_IS_FILTERS_FACTORY_FOR_IMPL_H
#define CPP_STREAM_DETAIL_IS_FILTERS_FACTORY_FOR_IMPL_H

#include "stream_traits_impl.h" // IsStreamFilterFor
#include <utility>              // ::std::declval
#include <type_traits>          // ::std::enable_if_t, ::std::false_type, ::std::true_type

namespace stream
{
    namespace detail
    {
        template<typename Factory, typename Stream, typename = void>
        struct IsFiltersFactoryFor : ::std::false_type
        {};

        template<typename Factory, typename Stream>
        struct IsFiltersFactoryFor<Factory, Stream, VoidT<::std::enable_if_t<IsStreamFilterFor<decltype(::std::declval<Factory>().template createFilter<Stream>()),
                                                                                               Stream>::value>>
                                  > : ::std::true_type
        {};
    }
}

#endif //CPP_STREAM_DETAIL_IS_FILTERS_FACTORY_FOR_IMPL_H
