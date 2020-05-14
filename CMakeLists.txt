cmake_minimum_required(VERSION 3.15)
project(comp-vision)

set(CMAKE_CXX_STANDARD 14)

add_executable(comp-vision main.cpp)

find_package(OpenCV REQUIRED core imgproc highgui)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui)
target_link_libraries(comp-vision ${OpenCV_LIBS})