cmake_minimum_required(VERSION 3.10)
project(dashlauncher)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/raylib)

add_executable(app
    src/main.c
)

target_link_libraries(app PRIVATE raylib)
