cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

# set(CMAKE_CXX_STANDARD 23)

find_package(agtb CONFIG REQUIRED)

function(create_new_executable target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} agtb::agtb)
endfunction()

# create_new_executable(Draft "src/Draft.cpp")

# create_new_executable(Adjustment_Traverse "src/Adjustment/Traverse.cpp")
# create_new_executable(Adjustment_Elevation "src/Adjustment/Elevation.cpp")
# create_new_executable(Adjustment_ElevationNet "src/Adjustment/ElevationNet.cpp")

# create_new_executable(Geodesy_Geometry "src/Geodesy/Ellipsoid/Geometry.cpp")
# create_new_executable(Geodesy_Projection_GaussKruger "src/Geodesy/Projection/GaussKruger.cpp")
# create_new_executable(Geodesy_Solution_Gauss "src/Geodesy/Solution/Gauss.cpp")
create_new_executable(Geodesy_Solution_Bessel "src/Geodesy/Solution/Bessel.cpp")

# # # # Not a test_package, but part for testing implement in dev
# # # create_new_executable(Geodesy_ExMath_PreCorrection_MeridianArcBottom "src/Geodesy/ExMath/PreCorrection_MeridianArcBottom.cpp")
# # # create_new_executable(Geodesy_ExMath_PreCorrection_GaussKruger "src/Geodesy/ExMath/PreCorrection_GaussKruger.cpp")


# create_new_executable(IO_Eigen "src/IO/Eigen.cpp")

# create_new_executable(Photographic_SpaceResection "src/Photogrammetry/SpaceResection.cpp")
# # # create_new_executable(Photographic_SpaceIntersection "src/Photogrammetry/SpaceIntersection.cpp") # Not Implement
# # # create_new_executable(Photographic_ContinuousRelativeOrientate "src/Photogrammetry/ContinuousRelativeOrientate.cpp") # Not Implement

# create_new_executable(Utils_Angle "src/Utils/Angles.cpp")
# create_new_executable(Utils_DataFrame "src/Utils/DataFrame.cpp")

