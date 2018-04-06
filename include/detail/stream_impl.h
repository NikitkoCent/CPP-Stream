#ifndef CPP_STREAM_DETAIL_STREAM_IMPL_H
#define CPP_STREAM_DETAIL_STREAM_IMPL_H

#include "stream_base.h"    // StreamBase
#include "traits_impl.h"    // VoidT, ContainerTraits, IsContainerV, IsGeneratorV, IsRangeV
#include <type_traits>      // ::std::remove_reference_t, ::std::enable_if_t, ::std::is_lvalue_reference, ::std::decay_t
#include <utility>          // ::std::move, ::std::forward
#include <initializer_list> // ::std::initializer_list
#include <functional>       // ::std::reference_wrapper

namespace stream
{
    namespace detail
    {
        template<typename T, typename Source, typename Derived, typename = void>
        class StreamImpl;


        // Container (non-reference)
        template<typename T, typename Container, typename Derived>
        class StreamImpl<T,
                         Container,
                         Derived,
                         VoidT<::std::enable_if_t<IsContainerV<Container>>,
                               ::std::enable_if_t<!::std::is_reference<Container>::value>>
                        > : public StreamBase<T, true, Derived>
        {
        public:
            using RealType = typename StreamBase<T, true, Derived>::RealType;


            StreamImpl() = default;

            StreamImpl(Container &&container)
                : container(::std::move(container))
            {}

            StreamImpl(::std::initializer_list<T> initList)
                : container(initList)
            {}

            template<typename Arg1, typename... Args>
            StreamImpl(Arg1 &&arg1, Args&&... args)
            {
                initialize(::std::forward<Arg1>(arg1), ::std::forward<Args>(args)...);
            }

        private:
            Container container;


            template<typename Arg1>
            void initialize(Arg1 &&arg1)
            {
                container.emplace_back(::std::forward<Arg1>(arg1));
            }

            template<typename Arg1, typename... Args>
            void initialize(Arg1 &&arg1, Args&&... args)
            {
                container.emplace_back(::std::forward<Arg1>(arg1));
                initialize(::std::forward<Args>(args)...);
            }
        };


        // Container reference
        template<typename T, typename ContainerRef, typename Derived>
        class StreamImpl<T,
                         ContainerRef,
                         Derived,
                         VoidT<::std::enable_if_t<IsContainerV<ContainerRef>>,
                               ::std::enable_if_t<::std::is_lvalue_reference<ContainerRef>::value>>
                        > : public StreamBase<T, true, Derived>
        {
        public:
            using RealType = typename StreamBase<T, true, Derived>::RealType;

            StreamImpl(ContainerRef containerRef)
                : containerRef(containerRef)
            {}

        private:
            ::std::reference_wrapper<::std::remove_reference_t<ContainerRef>> containerRef;
        };


        // Generator
        template<typename T, typename Generator, typename Derived>
        class StreamImpl<T,
                        Generator,
                        Derived,
                        VoidT<::std::enable_if_t<IsGeneratorV<Generator>>>
                        > : public StreamBase<T, false, Derived>
        {
        public:
            using RealType = typename StreamBase<T, false, Derived>::RealType;


            template<typename Callable>
            StreamImpl(Callable &&callable)
                : generator(::std::forward<Callable>(callable))
            {}

        private:
            ::std::decay_t<Generator> generator;
        };


        // Range
        template<typename T, typename Iterator, typename Derived>
        class StreamImpl<T,
                         Iterator,
                         Derived,
                         VoidT<::std::enable_if_t<IsRangeV<Iterator>>>
                        > : public StreamBase<T, true, Derived>
        {
        public:
            using RealType = typename StreamBase<T, true, Derived>::RealType;


            template<typename B, typename E>
            StreamImpl(B &&rangeBegin, E &&rangeEnd)
                : rangeBegin(::std::forward<B>(rangeBegin)), rangeEnd(::std::forward<E>(rangeEnd))
            {}

        private:
            Iterator rangeBegin;
            Iterator rangeEnd;
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_IMPL_H
