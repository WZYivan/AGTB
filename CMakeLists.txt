cmake_minimum_required(VERSION 3.15)
project(agtb CXX)

find_package(Eigen3 REQUIRED)
find_package(gcem REQUIRED)

add_library(agtb INTERFACE)
target_include_directories(agtb INTERFACE include)
target_link_libraries(agtb INTERFACE 
    Eigen3::Eigen
    gcem
)

install(DIRECTORY include/AGTB DESTINATION include)
