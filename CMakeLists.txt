cmake_minimum_required(VERSION 3.16.3)
project(API C)

set(CMAKE_C_STANDARD 11)

add_executable(API main.c)
set(GCC_COVERAGE_COMPILE_FLAGS "-DEVAL -std=gnu11 -pipe -lm")
set(ROBA_DI_CONTROLLO "-Wuninitialized -Wall -Werror=unused-result -pedantic -Werror -ggdb3 -Werror=implicit-function-declaration ")
#[[set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")]]
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${ROBA_DI_CONTROLLO}")
