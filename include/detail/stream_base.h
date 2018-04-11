#ifndef CPP_STREAM_DETAIL_STREAM_BASE_H
#define CPP_STREAM_DETAIL_STREAM_BASE_H

#include "traits_impl.h"
#include <type_traits>  // ::std::enable_if_t
#include <optional>     // ::std::optional
#include "../value_holder.h"

namespace stream
{
    template<typename, typename>
    struct Stream;


    namespace detail
    {
        template<typename OldStream, typename Contin>
        struct CombinedStreamTag;


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


            template<typename Contin, ::std::enable_if_t<IsContinuationForV<Contin, Derived>>* = nullptr>
            auto operator|(Contin &&contin)
            {
                using V = UnwrapValueHolderT<typename ContinuationForTraits<Contin, Derived>::ValueType>;
                using RetStreamT = stream::Stream<V, CombinedStreamTag<Derived, ::std::remove_reference_t<Contin>>>;

                return RetStreamT(::std::move(static_cast<Derived&>(*this)), ::std::forward<Contin>(contin));
            }


            template<typename Factory, ::std::enable_if_t<!IsContinuationForV<Factory, Derived>
                                                          && IsContinuationsFactoryForV<Factory, Derived>>* = nullptr>
            auto operator|(Factory &&factory)
            {
                return operator|(::std::forward<Factory>(factory).template createContinuation<Derived>());
            }


            template<typename Consumer, ::std::enable_if_t< !IsContinuationForV<Consumer, Derived>
                                                            && !IsContinuationsFactoryForV<Consumer, Derived>
                                                            && IsConsumerForV<Consumer, Derived> >* = nullptr>
            decltype(auto) operator|(Consumer &&consumer)
            {
                return ::std::forward<Consumer>(consumer)(::std::move(static_cast<Derived&>(*this)));
            }
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_BASE_H
