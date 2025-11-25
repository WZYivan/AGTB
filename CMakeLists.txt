cmake_minimum_required(VERSION 3.15)
project(agtb CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(Eigen3 REQUIRED)
find_package(gcem REQUIRED)
find_package(Boost REQUIRED COMPONENTS
    iostreams
)

add_library(agtb INTERFACE)
target_include_directories(agtb INTERFACE include)
target_link_libraries(agtb INTERFACE 
    Eigen3::Eigen
    gcem
    boost::iostreams
)

install(DIRECTORY include/AGTB DESTINATION include)
