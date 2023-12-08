cmake_minimum_required(VERSION 3.0)
project(3dRenderer)
set(CMAKE_CXX_STANDARD 11)

set(SFML_DIR "${CMAKE_SOURCE_DIR}/../SFML-2.6.0/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS system window graphics REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(SOURCES ${CMAKE_SOURCE_DIR}/../src/main.cpp)
add_executable(app ${SOURCES})

target_include_directories(app PRIVATE "${CMAKE_SOURCE_DIR}/../SFML-2.6.0/include")
target_link_libraries(app PRIVATE sfml-system sfml-window sfml-graphics)
#target_link_libraries(app PRIVATE "-mwindows -lwinmm -lgdi32")