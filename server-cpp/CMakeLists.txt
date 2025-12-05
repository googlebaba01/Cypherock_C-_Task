cmake_minimum_required(VERSION 3.20)
project(cypherock_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cypherock_server
    src/main.cpp
)

target_include_directories(cypherock_server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
