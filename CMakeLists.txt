#project name
PROJECT (ImgScale)

cmake_minimum_required(VERSION 2.8)

#save ImgScale to output file.
SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/output)

#CMAKE_CXX_FLAGS
#SET(CMAKE_CXX_FLAGS "-wl, -g")


#include
SET(PUBLIC_INCLUDE_PATH
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/bicubic/
    ${PROJECT_SOURCE_DIR}/bilinear/
    ${PROJECT_SOURCE_DIR}/nearest/
)

INCLUDE_DIRECTORIES(${PUBLIC_INCLUDE_PATH})


#link so
#SET(PUBLIC_SO libgstreamer-1.0.so libgobject-2.0.so.0 libglib-2.0.so.0)

#set src_list
SET(SRC_LIST  main.cpp
              nearest/nearest.cpp
              bilinear/bilinear.cpp
              bicubic/bicubic.cpp)

#exec
ADD_EXECUTABLE(ImgScale ${SRC_LIST})
#TARGET_LINK_LIBRARIES(mp4player ${PUBLIC_SO})


