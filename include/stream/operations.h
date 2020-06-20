#ifndef CPP_STREAM_OPERATIONS_H
#define CPP_STREAM_OPERATIONS_H

#include "stream.h"
#include "traits.h"
#include "continuation.h"
#include <cstddef>
#include <type_traits>


namespace stream
{
    namespace ops
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
                        oStream << val.value().get();
                        break;
                    }
                }

                while (!stream.isEnd())
                {
                    auto val = stream.getNext();
                    if (val)
                    {
                        oStream << delim << val.value().get();
                    }
                }

                return oStream;
            };
        }


        inline auto skip(::std::size_t amount)
        {
            return makeContinuation<false>([amount](auto &&value, auto &&stream) mutable {
                using RealType = typename stream::StreamTraits<decltype(stream)>::RealType;

                if (amount > 0) {
                    --amount;
                    return ::std::optional<RealType>{::std::nullopt};
                }

                return ::std::optional<RealType>{::std::forward<decltype(value)>(value)};
            });
        }


        template<typename Transform>
        auto map(Transform &&transform)
        {
            return makeContinuation<false>([transform = ::std::forward<Transform>(transform)](auto &&value, auto &&) mutable {
                return ::std::optional{transform(::std::move(value.get()))};
            });
        }


        inline auto get(::std::size_t n)
        {
            return makeContinuation<true>([n](auto &&value, auto &&stream, bool &end) mutable {
                using RealType = typename stream::StreamTraits<decltype(stream)>::RealType;
                if (n > 0)
                {
                    --n;
                    end = (n == 0);
                    return ::std::optional<RealType>{::std::forward<decltype(value)>(value)};
                }

                end = true;
                return ::std::optional<RealType>{::std::nullopt};
            });
        }


        template<typename Identity, typename Accumulator>
        auto reduce(Identity &&identity, Accumulator &&accumulator)
        {
            return [f1 = ::std::forward<Identity>(identity), fn = ::std::forward<Accumulator>(accumulator)](auto &&stream) mutable {
                using StreamValueType = typename StreamTraits<decltype(stream)>::ValueType;
                using ResultType = detail::InvokeResultT<decltype(f1), StreamValueType&&>;

                if (stream.isEnd())
                {
                    return ResultType{};
                }

                ResultType result{};
                while (!stream.isEnd())
                {
                    auto initializer = stream.getNext();
                    if (initializer)
                    {
                        result = f1(::std::move(initializer->get()));
                        break;
                    }
                }

                while (!stream.isEnd())
                {
                    auto forReduce = stream.getNext();
                    if (forReduce)
                    {
                        result = fn(::std::move(result), ::std::move(forReduce->get()));
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


        inline auto sum()
        {
            return reduce([](auto &&v1, auto &&v2) { return v1 + v2; });
        }


        inline auto nth(::std::size_t index)
        {
            return [index](auto &&stream) mutable {
                while (index > 0)
                {
                    if constexpr (StreamTraits<decltype(stream)>::IsFinite)
                    {
                        if (stream.isEnd())
                        {
                            throw ::std::out_of_range("Out of Stream range");
                        }
                    }

                    if (stream.getNext())
                    {
                        --index;
                    }
                }

                auto result = stream.getNext();
                while (!result)
                {
                    if constexpr (StreamTraits<decltype(stream)>::IsFinite)
                    {
                        if (stream.isEnd())
                        {
                            throw ::std::out_of_range("Out of Stream range");
                        }
                    }

                    result = stream.getNext();
                }

                return ::std::move(result->get());
            };
        }


        inline auto to_vector()
        {
            return [](auto &&stream) {
                using ValueType = typename StreamTraits<decltype(stream)>::ValueType;

                ::std::vector<ValueType> result;
                while (!stream.isEnd())
                {
                    auto value = stream.getNext();
                    if (value)
                    {
                        result.emplace_back(::std::move(value->get()));
                    }
                }

                return result;
            };
        }


        template<typename Predicate>
        auto filter(Predicate &&predicate)
        {
            return makeContinuation<false>([predicate = ::std::forward<Predicate>(predicate)](auto &&value, auto &&stream) {
                using RealType = typename StreamTraits<decltype(stream)>::RealType;
                if (predicate(static_cast<const RealType&>(value).get()))
                {
                    return ::std::optional<RealType>{::std::forward<decltype(value)>(value)};
                }

                return ::std::optional<RealType>{::std::nullopt};
            });
        }


        class group
        {
        public:
            group(::std::size_t n) : n(n) {}


            template<typename Stream, ::std::enable_if_t<StreamTraits<Stream>::IsFinite>* = nullptr>
            auto createContinuation()
            {
                using ValueType = typename StreamTraits<Stream>::ValueType;

                ::std::size_t localN = this->n;
                return makeContinuation<false>([localN, vec = ::std::vector<ValueType>{}](auto &&value, auto &&stream) mutable {
                    vec.emplace_back(::std::move(value.get()));

                    if ((vec.size() == localN) || stream.isEnd())
                    {
                        return ::std::optional<::std::vector<ValueType>>(::std::move(vec));
                    }

                    return ::std::optional<::std::vector<ValueType>>(::std::nullopt);
                });
            }

            template<typename Stream, ::std::enable_if_t<!StreamTraits<Stream>::IsFinite>* = nullptr>
            auto createContinuation()
            {
                using ValueType = typename StreamTraits<Stream>::ValueType;

                ::std::size_t localN = this->n;
                return makeContinuation<false>([localN, vec = ::std::vector<ValueType>{}](auto &&value, auto &&) mutable {
                    vec.emplace_back(::std::move(value.get()));

                    if (vec.size() == localN)
                    {
                        return ::std::optional<::std::vector<ValueType>>(::std::move(vec));
                    }

                    return ::std::optional<::std::vector<ValueType>>(::std::nullopt);
                });
            }

        private:
            ::std::size_t n;
        };
    }
}

#endif //CPP_STREAM_OPERATIONS_H
