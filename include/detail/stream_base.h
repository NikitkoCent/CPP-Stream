#ifndef CPP_STREAM_DETAIL_STREAM_BASE_H
#define CPP_STREAM_DETAIL_STREAM_BASE_H

#include "traits_impl.h"
#include <type_traits>  // ::std::enable_if_t
#include <optional>     // ::std::optional
#include "../value_holder.h"

namespace stream
{
    namespace detail
    {
        template<typename T, bool Finiteness, typename Derived>
        struct StreamBase
        {
            using ValueType = RemoveCVRefT<T>;
            static constexpr bool IsFinite = Finiteness;


            bool isEnd() const
            {
                static_assert(IsFinite, "This Stream is infinite");
                return static_cast<const Derived*>(this)->isEndImpl();
            }
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_BASE_H
