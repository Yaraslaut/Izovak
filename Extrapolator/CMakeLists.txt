cmake_minimum_required(VERSION 2.8.12)

project(Extrapolator)

set(Extrapolator_SOURCE 
	ext.cpp
  FitCurve.h
  FitCurve.cpp
	)

add_subdirectory(${CMAKE_SOURCE_DIR}/include/dlib dlib_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/spdlog spdlog)


add_library(Extrapolator ${Extrapolator_SOURCE})
target_link_libraries(Extrapolator dlib::dlib spdlog::spdlog )

