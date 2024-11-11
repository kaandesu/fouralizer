cmake_minimum_required(VERSION 3.0)
project(MyProject C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/raylib")

include_directories(${RAYLIB_DIR}/include)

add_executable(fouralizer src/main.c)

if(APPLE)
    target_link_libraries(fouralizer 
        "${RAYLIB_DIR}/libraylib.a" 
        "-framework Cocoa" 
        "-framework OpenGL" 
        "-framework IOKit" 
        "-framework CoreVideo" 
        ${CMAKE_DL_LIBS} # Dynamically linked libraries (if needed)
    )
else()
    # Linux specific settings (if applicable)
    target_link_libraries(fouralizer "${RAYLIB_DIR}/libraylib.a")
endif()
