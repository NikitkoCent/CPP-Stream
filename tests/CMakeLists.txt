option(COLLECT_CODE_COVERAGE_LCOV "Setup code coverage or not" OFF)

find_package(Threads)

include(${cmake-modules_SOURCE_DIR}/EnableExtraCompilerWarnings.cmake)

enable_testing()

find_package(Threads)

add_executable(unittesting ${_cpp-stream_INCLUDE_DIR}/stream.h
                           ${_cpp-stream_INCLUDE_DIR}/traits.h
                           ${_cpp-stream_INCLUDE_DIR}/value_holder.h
                           ${_cpp-stream_INCLUDE_DIR}/continuation.h
                           ${_cpp-stream_INCLUDE_DIR}/operations.h
                           ${_cpp-stream_INCLUDE_DIR}/detail/stream_base.h
                           ${_cpp-stream_INCLUDE_DIR}/detail/traits_impl.h
                           ${_cpp-stream_INCLUDE_DIR}/detail/stream_impl.h
                           ${_cpp-stream_INCLUDE_DIR}/detail/continuation_impl.h
                           stream_deduction_guides.cpp
                           filters_int_finite_stream.cpp
                           filters_int_infinite_stream.cpp
                           filters_generic_int_stream.cpp
                           filters_move_only_finite_stream.cpp
                           filters_move_only_infinite_stream.cpp
                           filters_generic_move_only_stream.cpp
                           noisy.h
                           noisy.cpp
                           filters_noisy_finite_stream.cpp
                           filters_generic_noisy_stream.cpp)

if (COLLECT_CODE_COVERAGE_LCOV)
    include(${cmake-modules_SOURCE_DIR}/CodeCoverage.cmake)

    append_target_coverage_flags(unittesting)
    setup_target_for_coverage_lcov(NAME collect_coverage
                                   EXECUTABLE unittesting
                                   DEPENDENCIES unittesting)
endif (COLLECT_CODE_COVERAGE_LCOV)
enable_extra_compiler_warnings(unittesting)

target_link_libraries(unittesting gtest gmock_main Threads::Threads)

add_test(NAME filters_test COMMAND unittesting --gtest_filter=FILTER*)
add_test(NAME deduction_guides_test COMMAND unittesting --gtest_filter=DEDUCTION_GUIDES*)
