cmake_minimum_required(VERSION 2.8)

project(ccc LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_library(ccc_lexer OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.cpp)

add_executable(ccc_lexer_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/lexer_test.cpp)
target_link_libraries(ccc_lexer_test ccc_lexer)
