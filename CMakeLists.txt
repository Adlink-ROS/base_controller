cmake_minimum_required(VERSION 3.5)

project(base_controller)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(serial REQUIRED)

set(target ${PROJECT_NAME})
set(source_path ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(header_path ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${target} SHARED
  ${source_path}/controllers.cpp
  )

target_link_libraries(${target}
  PUBLIC
  ${SERIAL_LIBRARIES}
  )
target_include_directories(${target}
  PUBLIC
  ${header_path}
  ${SERIAL_INCLUDE_DIRS}
  )

install(TARGETS
  ${target}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )

## Install headers
install(
  DIRECTORY ${header_path}/${PROJECT_NAME}
  DESTINATION include
  )

## Install CMake config
install(
  FILES cmake/${PROJECT_NAME}Config.cmake
  DESTINATION share/${PROJECT_NAME}/cmake
  )


## Install package.xml
install(
  FILES package.xml
  DESTINATION share/${PROJECT_NAME}
  )
