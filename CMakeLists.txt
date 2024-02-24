cmake_minimum_required(VERSION 3.4.3)

project(ccc LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_library(ccc_utility OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/utility.cpp)

add_library(ccc_lexer OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.cpp)

add_library(ccc_parser OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp)

add_library(ccc_vm OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/src/vm.cpp)

add_executable(ccc ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(ccc ccc_utility ccc_lexer ccc_parser ccc_vm)

add_executable(ccc_integration_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.cpp)
target_link_libraries(ccc_integration_test ccc_utility ccc_lexer ccc_parser ccc_vm)
