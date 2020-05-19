cmake_minimum_required(VERSION 3.15)
project(comp-vision)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)

add_executable(comp-vision main.cpp)
add_executable(contourUtils contourUtils.cpp)
add_executable(drawing drawing.cpp)
add_executable(grid grid.cpp)
add_executable(myDP myDP.cpp)
add_executable(projection projection.cpp)
add_executable(rotation rotation.cpp)
add_executable(iou iou.cpp)

find_package(OpenCV REQUIRED core imgproc highgui)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui)

find_package(CGAL REQUIRED COMPONENTS Core)
include( ${CGAL_USE_FILE} )

set(ALL_LIBS ${OpenCV_LIBS} CGAL::CGAL CGAL::CGAL_Core)

target_link_libraries(comp-vision ${ALL_LIBS})
target_link_libraries(rotation ${ALL_LIBS})
target_link_libraries(grid ${ALL_LIBS})
target_link_libraries(contourUtils ${ALL_LIBS})
target_link_libraries(drawing ${ALL_LIBS})
target_link_libraries(myDP ${ALL_LIBS})
target_link_libraries(projection ${ALL_LIBS})
target_link_libraries(iou ${ALL_LIBS})