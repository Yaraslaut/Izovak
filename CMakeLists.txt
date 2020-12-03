cmake_minimum_required(VERSION 2.8.12)

project(FitData)

if(LOG)
   set(USE_logging ON)
   message("-- Logging is ON")
else()
  set(USE_logging OFF)
  message("-- Logging is OFF")
endif()

set(FitData_SOURCE 
	test.cpp
	)

add_subdirectory(${CMAKE_SOURCE_DIR}/Extrapolator)

add_executable(FitData ${FitData_SOURCE})

if(USE_logging)
target_compile_definitions(FitData PUBLIC USE_SPDLOG)
endif()


target_link_libraries(FitData Extrapolator)
