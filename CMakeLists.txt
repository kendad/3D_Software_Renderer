cmake_minimum_required(VERSION 3.28)

project(
  SoftwareRasterizer C
  VERSION 1.0.0
  DESCRIPTION "3D software rasterizer build in C"
  LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compilation commands for LSP/IDE integration." FORCE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type" FORCE)
endif()

set(SOURCE_FILES
  src/main.c
  src/vector.c
)

add_compile_options(
  -Wall
)

find_package(SDL2 REQUIRED)

add_executable(renderer ${SOURCE_FILES})

target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(renderer PUBLIC
  SDL2::SDL2
  m
)

install(TARGETS renderer DESTINATION bin)
