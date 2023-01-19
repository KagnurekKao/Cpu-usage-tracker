cmake_minimum_required(VERSION 3.0.0)

#set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_COMPILER "/usr/bin/gcc")

project(CUT VERSION 0.1.0)



include(CTest)
enable_testing()

add_executable(CUT EXCLUDE_FROM_ALL main.c)
target_compile_options(${PROJECT_NAME} PRIVATE -pthread -Wall -Wextra)
#target_compile_options(${PROJECT_NAME} PRIVATE -pthread -Weverything)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


