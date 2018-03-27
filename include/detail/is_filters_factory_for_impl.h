#ifndef CPP_STREAM_DETAIL_IS_FILTERS_FACTORY_FOR_IMPL_H
#define CPP_STREAM_DETAIL_IS_FILTERS_FACTORY_FOR_IMPL_H

#include "stream_traits_impl.h" // IsStreamFilterFor
#include <utility>              // ::std::declval

namespace stream
{
    namespace detail
    {
        template<typename Factory, typename Stream>
        struct IsFiltersFactoryFor : IsStreamFilterFor<decltype(::std::declval<Factory>().createFilter<Stream>()),
                                                       Stream>
        {};
    }
}

#endif //CPP_STREAM_DETAIL_IS_FILTERS_FACTORY_FOR_IMPL_H
