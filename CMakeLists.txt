cmake_minimum_required(VERSION 3.24)
project(montecarlo)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(matplotplusplus)
add_executable(montecarlo main.cpp integrator.h vec.h)

target_link_libraries(montecarlo PUBLIC matplot)

