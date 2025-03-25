cmake_minimum_required(VERSION 3.10)

project(wality)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw")
set(GLM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")
set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/stb")
set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/ffmpeg")

find_package(OpenGL REQUIRED)

add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    includes 
    ${GLFW_DIR}/include
    ${GLAD_DIR}/include
    ${GLM_DIR}
    ${STB_DIR}
    ${FFMPEG_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE 
    ${GLFW_DIR}/lib-mingw-w64
    ${FFMPEG_DIR}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    glfw3
    OpenGL::GL
    avcodec
    avformat
    avutil
    swscale
    swresample
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLFW_DIR}/lib-mingw-w64/glfw3.dll"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

file(GLOB FFMPEG_DLLS "${FFMPEG_DIR}/bin/*.dll")
file(COPY ${FFMPEG_DLLS} DESTINATION ${CMAKE_BINARY_DIR})