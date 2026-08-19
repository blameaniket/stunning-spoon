cmake_minimum_required(VERSION 3.10)
project(stunning-spoon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(app
    src/main.c
    src/launcher.c
    src/window.c
    src/renderer.c
    src/log.c
    src/color.c
    src/config_file.c
    external/glad/src/glad.c
)


target_include_directories(app PRIVATE
    src
    external/glad/include
    ${FREETYPE_INCLUDE_DIRS}
)


find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(app PRIVATE
    glfw
    OpenGL::GL
    Freetype::Freetype
    m
)


# debug build
target_compile_definitions(app PRIVATE DEBUG)
