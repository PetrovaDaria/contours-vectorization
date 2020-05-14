cmake_minimum_required(VERSION 3.15)
project(comp-vision)

set(CMAKE_CXX_STANDARD 14)

add_executable(comp-vision main.cpp)
add_executable(contourUtils contourUtils.cpp)
add_executable(drawing drawing.cpp)
add_executable(grid grid.cpp)
add_executable(myDP myDP.cpp)

find_package(OpenCV REQUIRED core imgproc highgui)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui)
target_link_libraries(comp-vision ${OpenCV_LIBS})
target_link_libraries(contourUtils ${OpenCV_LIBS})
target_link_libraries(drawing ${OpenCV_LIBS})
target_link_libraries(grid ${OpenCV_LIBS})
target_link_libraries(myDP ${OpenCV_LIBS})