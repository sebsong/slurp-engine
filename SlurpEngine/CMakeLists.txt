cmake_minimum_required(VERSION 3.26)
project(SlurpEngine)
set(CMAKE_CXX_STANDARD 20)

include_directories(Engine/Public)
include_directories(Engine/Private)
add_subdirectory(Engine/Private)