cmake_minimum_required(VERSION 3.18)
project(cpputils_tests)

include(FetchContent)
FetchContent_Declare(
  ut
  GIT_REPOSITORY https://github.com/boost-ext/ut
  GIT_TAG v1.1.9
)
FetchContent_MakeAvailable(ut)

function (cpputils_add_test NAME SOURCES)
    add_executable(${NAME} ${SOURCES})
    add_test(NAME ${NAME} COMMAND ./${NAME})
    target_link_libraries(${NAME} PRIVATE Boost::ut)
    target_include_directories(${NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../src)
endfunction()

enable_testing()
cpputils_add_test(test_type_traits test_type_traits.cpp)
