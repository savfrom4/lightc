cmake_minimum_required(VERSION 3.10)
project(lcjit)

# Set compiler-specific flags
if(MSVC)
    set(FM_PYTHON_EXEC "python" CACHE STRING "Python executable path")

    set(CMAKE_C_FLAGS "/W4 /EHsc")
    set(CMAKE_C_FLAGS_DEBUG "/ZI")
    set(CMAKE_C_FLAGS_RELEASE "/O2")

    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:/ZI>")
    target_link_options(${PROJECT_NAME} PUBLIC "$<$<CONFIG:DEBUG>:/OPT:NOICF>" "$<$<CONFIG:DEBUG>:/OPT:NOREF>")
else()
    set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-parentheses")
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
endif()

add_library(lcjit
    "src/lc_error.c"
    "src/lc_types.c"    
    "src/lc_mem.c"
    "src/lc_vm.c"
    "src/lc_tokenizer.c"
    "src/lc_compiler.c"
)

target_include_directories(lcjit PUBLIC "include/")
target_link_libraries(lcjit PUBLIC m)

project(lcjit-test)

add_executable(lcjit-test 
    "src/main.c"
)

target_link_libraries(lcjit-test PUBLIC lcjit)