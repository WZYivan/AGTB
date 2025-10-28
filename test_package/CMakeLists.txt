cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(agtb CONFIG REQUIRED)

function(create_new_executable target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} agtb::agtb)
endfunction()

create_new_executable(SpaceResection "src/SpaceResection.cpp")
# create_new_executable(EigenIO "src/IO_EigenIO.cpp")
# create_new_executable(StringUtils "src/Utils_String.cpp")