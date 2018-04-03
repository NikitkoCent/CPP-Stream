cmake_minimum_required(VERSION 3.8.2)
project(CPP-Stream)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

find_package(Threads)

if (CMAKE_VERSION VERSION_LESS 3.2)
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
    set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()
include(third-party/DownloadProject/DownloadProject.cmake)

# =============================== CMAKE MODULES =========================================
download_project(PROJ                cmake-modules
                 GIT_REPOSITORY      https://github.com/NikitkoCent/cmake-modules
                 GIT_TAG             changes
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
                )
# =============================== END CMAKE MODULES =====================================

# ================================ SETUP WARNINGS =======================================
include(${cmake-modules_SOURCE_DIR}/EnableExtraCompilerWarnings.cmake)
globally_enable_extra_compiler_warnings()
# ============================== END SETUP WARNINGS =====================================

# ================================== GOOGLETEST =========================================
enable_testing()

download_project(PROJ                googletest
                 GIT_REPOSITORY      https://github.com/google/googletest.git
                 GIT_TAG             master
                 ${UPDATE_DISCONNECTED_IF_AVAILABLE}
                )

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include")
endif()
# ================================ GOOGLETEST END =======================================

add_executable(unittesting include/traits.h
                           include/stream.h
                           include/stream_traits.h
                           include/filters_lib.h
                           include/filter.h
                           include/is_filters_factory_for.h
                           include/detail/stream_impl.h
                           include/detail/traits_impl.h
                           include/detail/stream_base.h
                           include/detail/utility.h
                           include/detail/stream_traits_impl.h
                           include/detail/stream_filter_impl.h
                           include/detail/filter_impl.h
                           include/detail/is_filters_factory_for_impl.h
                           tests/stream_deduction_guides.cpp
                           tests/filters_int_finite_stream.cpp
                           tests/filters_int_infinite_stream.cpp
                           tests/filters_generic_int_stream.cpp
                           tests/filters_move_only_finite_stream.cpp
                           tests/filters_move_only_infinite_stream.cpp
                           tests/filters_generic_move_only_stream.cpp
                           tests/noisy.h tests/filters_noisy_finite_stream.cpp)
target_link_libraries(unittesting gtest gmock_main Threads::Threads)

add_test(NAME do_unittests COMMAND unittesting)

# ================================= TEST COVERAGE =======================================
#[[if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(${cmake-modules_SOURCE_DIR}/CodeCoverage.cmake)

    if (CMAKE_CXX_FLAGS_COVERAGE)
        APPEND_COVERAGE_COMPILER_FLAGS()
        SETUP_TARGET_FOR_COVERAGE(NAME collect_coverage
                                  EXECUTABLE ctest
                                  DEPENDENCIES unittesting)
    endif()
endif()]]
# ===============================  END TEST COVERAGE ====================================

MESSAGE("Build type : ${CMAKE_BUILD_TYPE}")
MESSAGE("Used compiler : ${CMAKE_CXX_COMPILER}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    MESSAGE("Compiler flags : ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
    MESSAGE("Linker flags : ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    MESSAGE("Compiler flags : ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
    MESSAGE("Linker flags : ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    MESSAGE("Compiler flags : ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    MESSAGE("Linker flags : ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    MESSAGE("Compiler flags : ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_MINSIZEREL}")
    MESSAGE("Linker flags : ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")
else()
    MESSAGE("Compiler flags : ${CMAKE_CXX_FLAGS}")
    MESSAGE("Linker flags : ${CMAKE_EXE_LINKER_FLAGS}")
endif()
