#ifndef CPP_STREAM_DETAIL_STREAM_BASE_H
#define CPP_STREAM_DETAIL_STREAM_BASE_H

#include <type_traits>  // ::std::is_reference, ::std::remove_reference, ::std::conditional_t, ::std::remove_const_t

namespace stream
{
    namespace detail
    {
        template<typename T, bool Finiteness, typename StreamImpl>
        struct StreamBase
        {
            using ValueType = T;
            using RealType = ::std::conditional_t<::std::is_reference<T>::value,
                                                  ::std::reference_wrapper<::std::remove_reference_t<T>>,
                                                  T>;
            static constexpr bool IsFinite = Finiteness;


            auto begin()
            {
                return static_cast<StreamImpl*>(this)->beginImpl();
            }

            auto end()
            {
                static_assert(IsFinite, "This Stream is infinite");
                return static_cast<StreamImpl*>(this)->endImpl();
            }
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_BASE_H
