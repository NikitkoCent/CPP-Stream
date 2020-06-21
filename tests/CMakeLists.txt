# Possible values:
#   * LCOV  - add coverage flags to compiler/linker
#             and setup target for code coverage collection via lcov
#   * GCOV  - add coverage flags to compiler/linker
#             and setup target for code coverage collection via gcov
#   * OTHER - add coverage flags to compiler/linker
#   * OFF   - do nothing
set(COLLECT_CODE_COVERAGE "OFF" CACHE STRING "Code coverage mode (LCOV|GCOV|OTHER|OFF)")

find_package(Threads)

include(${cmake-modules_SOURCE_DIR}/EnableExtraCompilerWarnings.cmake)

add_executable(unittesting ${_cpp-stream_INCLUDE_DIR}/stream/stream.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/traits.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/value_holder.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/continuation.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/operations.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/detail/stream_base.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/detail/traits_impl.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/detail/stream_impl.h
                           ${_cpp-stream_INCLUDE_DIR}/stream/detail/continuation_impl.h
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

if (COLLECT_CODE_COVERAGE STREQUAL "LCOV")
    include(${cmake-modules_SOURCE_DIR}/CodeCoverage.cmake)

    append_target_coverage_flags(unittesting)
    setup_target_for_coverage_lcov(NAME collect_coverage
                                   EXECUTABLE unittesting
                                   DEPENDENCIES unittesting)
elseif (COLLECT_CODE_COVERAGE STREQUAL "GCOV")
    message(FATAL_ERROR "GCOV value for COLLECT_CODE_COVERAGE variable isn't supported yet")
elseif (COLLECT_CODE_COVERAGE STREQUAL "OTHER")
    include(${cmake-modules_SOURCE_DIR}/CodeCoverage.cmake)

    append_target_coverage_flags(unittesting)
endif ()

enable_extra_compiler_warnings(unittesting)

target_link_libraries(unittesting gtest gmock_main Threads::Threads)

add_test(NAME filters_test COMMAND unittesting --gtest_filter=FILTER*)
add_test(NAME deduction_guides_test COMMAND unittesting --gtest_filter=DEDUCTION_GUIDES*)
