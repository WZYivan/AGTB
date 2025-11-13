cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(agtb CONFIG REQUIRED)

function(create_new_executable target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} agtb::agtb)
endfunction()

# create_new_executable(Draft "src/Draft.cpp")

# create_new_executable(Adjustment_Traverse "src/Adjustment/Traverse.cpp")
# create_new_executable(Adjustment_Elevation "src/Adjustment/Elevation.cpp")

# create_new_executable(Geodesy_Geometry "src/Geodesy_V2/Ellipsoid/Geometry.cpp")
create_new_executable(Geodesy_Projection_GaussKruger "src/Geodesy/Projection/GaussKruger.cpp")

# create_new_executable(IO_Eigen "src/IO/Eigen.cpp")

# create_new_executable(Photographic_SpaceResection "src/Photogrammetry/SpaceResection.cpp")
# create_new_executable(Photographic_SpaceIntersection "src/Photogrammetry/SpaceIntersection.cpp") # Not Implement
# create_new_executable(Photographic_ContinuousRelativeOrientate "src/Photogrammetry/ContinuousRelativeOrientate.cpp") # Not Implement

# create_new_executable(Utils_Angle "src/Utils/Angles.cpp")
# create_new_executable(Utils_String "src/Utils/String.cpp")

