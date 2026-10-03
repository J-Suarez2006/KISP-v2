cmake_minimum_required(VERSION 3.10.0)
project(KISP-v2 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(KISP-v2 main.cpp)
target_sources(KISP-v2 PRIVATE
    src/parser/parser.cpp
    src/error.hpp
    src/scanner/scanner.cpp
)
target_include_directories(KISP-v2 
PRIVATE 
    src
    src/interpreter 
    src/parser 
    src/scanner 
    src/semantic-analyzer
)

include(CTest)
enable_testing()

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
