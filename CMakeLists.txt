cmake_minimum_required(VERSION 3.10)
project(lcjit)

add_library(lcjit
    "src/lc_types.c"
)

target_include_directories(lcjit PUBLIC "include/")
target_link_libraries(lcjit PUBLIC m)