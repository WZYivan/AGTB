cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(agtb CONFIG REQUIRED)

function(create_new_executable target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} agtb::agtb)
endfunction()

# create_new_executable(SpaceResection "src/SpaceResection.cpp")
# create_new_executable(EigenIO "src/IO_EigenIO.cpp")
# create_new_executable(StringUtils "src/Utils_String.cpp")
# create_new_executable(Geodesy_Base "src/Geodesy_Base.cpp")
# create_new_executable(Bessel_Coeff "src/Bessel_Coeff.cpp")
# create_new_executable(Geodesy_PC "src/Geodesy_PC.cpp")
create_new_executable(MeridianArc_Part "src/MeridianArc_Part.cpp")
# create_new_executable(AngleUtils "src/Utils_Angles.cpp")
# create_new_executable(Adj_Traverse_ClosedLoop "src/Adj_Traverse_ClosedLoop.cpp")


