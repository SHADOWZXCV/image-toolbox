cmake_minimum_required(VERSION 3.10)
project(toolbox LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------- Assets ----------------
file(COPY ${CMAKE_SOURCE_DIR}/assets/fonts DESTINATION ${CMAKE_BINARY_DIR}/include/assets)

# ---------------- ImGui ----------------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)
# ---------------- ImPlot ----------------
set(IMPLOT_DIR ${CMAKE_SOURCE_DIR}/external/implot)
file(GLOB IMPLOT_SOURCES
    ${IMPLOT_DIR}/*.cpp
)

# ---------------- TinyFileDialogs ----------------
set(TINY_DIALOG_DIR ${CMAKE_SOURCE_DIR}/external//libtinyfiledialogs)
set(TINY_DIALOG_SOURCES ${TINY_DIALOG_DIR}/tinyfiledialogs.c)
add_library(tinyfiledialogs STATIC ${TINY_DIALOG_SOURCES})
set_target_properties(tinyfiledialogs PROPERTIES LINKER_LANGUAGE C)

# ---------------- OpenCV ----------------
set(OpenCV_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/opencv/include")
set(OpenCV_LIB_RELEASE "${CMAKE_SOURCE_DIR}/external/opencv/lib/opencv_world4120.lib")
set(OpenCV_LIB_DEBUG   "${CMAKE_SOURCE_DIR}/external/opencv/lib/opencv_world4120d.lib")
# ---------------- SDL2 ----------------
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/external/SDL2")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
link_directories(${SDL2_DIR}/lib/x64)
include_directories(${SDL2_INCLUDE_DIR})
# ---------------- SDL2_image ----------------
set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/external/SDL2_image")
set(SDL2_IMAGE_INCLUDE_DIR "${SDL2_IMAGE_DIR}/include")
link_directories(${SDL2_IMAGE_DIR}/lib/x64)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
# ---------------- Executable ----------------
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(toolbox main.cpp ${SRC_FILES} ${IMGUI_SOURCES} ${IMPLOT_SOURCES})

target_compile_definitions(toolbox PRIVATE SDL_MAIN_HANDLED)

target_include_directories(toolbox PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMPLOT_DIR}
    ${TINY_DIALOG_DIR}
    ${OpenCV_INCLUDE_DIR}
)

target_link_libraries(toolbox
    SDL2
    SDL2_image
    tinyfiledialogs
    debug   ${OpenCV_LIB_DEBUG}
    optimized ${OpenCV_LIB_RELEASE}
)
