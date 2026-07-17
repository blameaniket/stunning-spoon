cmake_minimum_required(VERSION 3.10)
project(dashlauncher)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FONTCONFIG REQUIRED fontconfig)

add_subdirectory(external/raylib)

add_executable(app
    src/main.c
    src/launcher.c
)

target_include_directories(app PRIVATE
    ${FONTCONFIG_INCLUDE_DIRS}
)

target_link_libraries(app PRIVATE
    raylib
    ${FONTCONFIG_LIBRARIES}
)
