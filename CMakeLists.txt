cmake_minimum_required(VERSION 3.10)

project(stunning-spoon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(app
    src/main.c
    src/launcher.c
    src/window.c
    external/glad/src/glad.c
)


find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)

target_include_directories(app PRIVATE
    src
    external/glad/include
)


target_link_libraries(app PRIVATE
    glfw
    OpenGL::GL
    Freetype::Freetype
)

# Unix/Linux needs libm.
# MSVC/Windows provides the math functions through the C runtime,
# so there is no "m.lib".
if(NOT WIN32)
    target_link_libraries(app PRIVATE m)
endif()

# Windows-specific libraries.
if(WIN32)
    target_link_libraries(app PRIVATE
        Psapi
    )
endif()


target_compile_definitions(app PRIVATE DEBUG)


if(MSVC)
    # Disable warnings about Microsoft's "unsafe" CRT functions such as
    # getenv() and sscanf().
    target_compile_definitions(app PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
endif()


# Format source files with clang-format.
find_program(CLANG_FORMAT clang-format REQUIRED)

set(FORMAT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/launcher.c
    ${CMAKE_SOURCE_DIR}/src/window.c
)

add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Formatting C source files with clang-format"
)
