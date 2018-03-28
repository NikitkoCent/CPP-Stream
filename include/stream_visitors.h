#ifndef CPP_STREAM_STREAM_VISITORS_H
#define CPP_STREAM_STREAM_VISITORS_H

#include "stream_traits.h"              // StreamValueT
#include "traits.h"                     // detail::InvokeResultT
#include "filter.h"                     // makeFilter
#include <cstddef>                      // ::std::size_t
#include <optional>                     // ::std::optional
#include <utility>                      // ::std::move, ::std::forward
#include <stdexcept>                    // ::std::out_of_range
#include <type_traits>                  // ::std::enable_if_t, ::std::conditional, ::std::is_reference, ::std::remove_reference_t


namespace stream
{
    namespace visitors
    {
        template<typename OStream>
        auto print_to(OStream &oStream, const char *delim = " ")
        {
            return [&oStream, delim](auto &&stream) -> OStream& {
                while (!stream.isEnd())
                {
                    auto val = stream.getNext();
                    if (val)
                    {
                        oStream << val.value() << delim;
                    }
                }

                return oStream;
            };
        }


        auto skip(::std::size_t amount)
        {
            return makeFilter<false>([amount](auto &&value, auto &&stream, bool&) mutable {
                using Type = stream::StreamValueT<decltype(stream)>;
                //using Type = int;

                if (amount > 0) {
                    --amount;
                    return ::std::optional<Type>{::std::nullopt};
                }

                return ::std::optional<Type>{::std::forward<decltype(value)>(value)};
            });
        }


        template<typename Transform>
        auto map(Transform &&transform)
        {
            return makeFilter<false>([transform = ::std::forward<Transform>(transform)](auto &&value, auto &&, bool&) mutable {
                using ReturnType = detail::InvokeResultT<::std::decay_t<Transform>, decltype(value)>;
                using Type = ::std::conditional_t<::std::is_reference<ReturnType>::value,
                                                  ::std::reference_wrapper<::std::remove_reference_t<ReturnType>>,
                                                  ReturnType>;

                return ::std::optional<Type>{transform(::std::forward<decltype(value)>(value))};
            });
        }


        auto get(::std::size_t n)
        {
            return makeFilter<true>([n](auto &&value, auto &&stream, bool &end) mutable {
                using Type = stream::StreamValueT<decltype(stream)>;
                if (n > 0)
                {
                    --n;
                    end = (n == 0);
                    return ::std::optional<Type>{::std::forward<decltype(value)>(value)};
                }

                end = true;

                return ::std::optional<Type>{::std::nullopt};
            });
        }


        template<typename Identity, typename Accumulator>
        auto reduce(Identity &&identity, Accumulator &&accumulator)
        {
            return [f1 = ::std::forward<Identity>(identity), fn = ::std::forward<Accumulator>(accumulator)](auto &&stream) mutable {
                using Type = detail::InvokeResultT<decltype(f1), StreamValueT<decltype(stream)>&&>;

                if (stream.isEnd())
                {
                    return Type{};
                }

                Type result;
                while (!stream.isEnd())
                {
                    auto initializer = stream.getNext();
                    if (initializer)
                    {
                        result = f1(*::std::move(initializer));
                        break;
                    }
                }

                while (!stream.isEnd())
                {
                    auto forReduce = stream.getNext();
                    if (forReduce)
                    {
                        result = fn(::std::move(result), *::std::move(forReduce));
                    }
                }

                return result;
            };
        }

        template<typename Accumulator>
        auto reduce(Accumulator &&accumulator)
        {
            return reduce([](auto &&elem){ return ::std::forward<decltype(elem)>(elem); },
                          ::std::forward<Accumulator>(accumulator));
        }


        auto sum()
        {
            return reduce([](auto &&v1, auto &&v2) { return v1 + v2; });
        }


        auto nth(::std::size_t index)
        {
            return [index](auto &&stream) mutable {
                while (!stream.isEnd())
                {
                    auto result = stream.getNext();
                    if (result)
                    {
                        if (index == 0)
                        {
                            return *::std::move(result);
                        }

                        --index;
                    }
                }

                throw ::std::out_of_range("Out of Stream range");
            };
        }


        auto to_vector()
        {
            return [](auto &&stream) {
                using Type = StreamValueT<decltype(stream)>;

                ::std::vector<Type> result;
                while (!stream.isEnd())
                {
                    auto value = stream.getNext();
                    if (value)
                    {
                        result.emplace_back(*::std::move(value));
                    }
                }

                return result;
            };
        }


        template<typename Predicate>
        auto filter(Predicate &&predicate)
        {
            return makeFilter<false>([predicate = ::std::forward<Predicate>(predicate)](auto &&value, auto &&stream, bool&) {
                using Type = StreamValueT<decltype(stream)>;
                if (predicate(static_cast<const RemoveCRefT<decltype(value)>&>(value)))
                {
                    return ::std::optional<Type>{::std::forward<decltype(value)>(value)};
                }

                return ::std::optional<Type>{::std::nullopt};
            });
        }


        class group
        {
        private:
            ::std::size_t n;

        public:
            group(::std::size_t n) : n(n) {}

            template<typename Stream>
            auto createFilter()
            {
                using Type = StreamValueT<Stream>;

                ::std::size_t n = this->n;
                return makeFilter<false>([n, vec = ::std::vector<Type>{}](auto &&value, auto &&, bool&) mutable {
                    if (vec.size() == n)
                    {
                        return ::std::optional<::std::vector<Type>>(::std::move(vec));
                    }

                    vec.emplace_back(::std::forward<decltype(value)>(value));

                    if (vec.size() == n)
                    {
                        return ::std::optional<::std::vector<Type>>(::std::move(vec));
                    }

                    return ::std::optional<::std::vector<Type>>(::std::nullopt);
                });
            }
        };
    }
}

#endif //CPP_STREAM_STREAM_VISITORS_H
