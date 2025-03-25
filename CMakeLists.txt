cmake_minimum_required(VERSION 3.10)
project(lcjit)
project(lcjit-test)

add_library(lcjit
    "src/lc_error.c"
    "src/lc_types.c"
    "src/lc_mem.c"
    "src/lc_vm.c"
)

target_include_directories(lcjit PUBLIC "include/")
target_link_libraries(lcjit PUBLIC m)

add_executable(lcjit-test 
    "src/main.c"
)

target_link_libraries(lcjit-test PUBLIC lcjit)