#ifndef CPP_STREAM_DETAIL_VISITOR_H
#define CPP_STREAM_DETAIL_VISITOR_H

#include "utility.h"
#include <type_traits>

namespace stream
{
    namespace detail
    {
        template<typename Callable, typename Stream>
        class Visitor
        {
        public:
            Visitor(const Callable &callable)
                : callable(callable)
            {}

            Visitor(Callable &&callable)
                : callable(::std::move(callable))
            {}


            InvokeResultT<Callable&, const Stream&> operator()(const Stream &stream)
            {
                if constexpr (::std::is_same<void, InvokeResultT<Callable&, const Stream&>>::value)
                {
                    callable(stream);
                }
                else
                {
                    return callable(stream);
                }
            }


            template<typename ThenVisitor>
            auto operator|(ThenVisitor &&then)
            {
                if constexpr (::std::is_same<void, InvokeResultT<Visitor<Callable, Stream>&, const Stream&>>::value)
                {
                    auto lambda = [now = ::std::move(*this), after = ::std::forward<ThenVisitor>(then)](const Stream &stream) mutable {
                        now(stream);
                        return after(stream);
                    };

                    return Visitor<decltype(lambda), Stream>(::std::move(lambda));
                }
                else
                {
                    auto lambda = [now = ::std::move(*this), after = ::std::forward<ThenVisitor>(then)](const Stream &stream) mutable {
                        return after(now(stream));
                    };

                    return Visitor<decltype(lambda), Stream>(::std::move(lambda));
                }
            }


        private:
            ::std::decay_t<Callable> callable;
        };
    }
}

#endif //CPP_STREAM_DETAIL_VISITOR_H
