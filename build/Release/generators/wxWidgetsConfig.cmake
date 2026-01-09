########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(wxWidgets_FIND_QUIETLY)
    set(wxWidgets_MESSAGE_MODE VERBOSE)
else()
    set(wxWidgets_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/wxWidgetsTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${wxwidgets_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(wxWidgets_VERSION_STRING "3.2.8")
set(wxWidgets_INCLUDE_DIRS ${wxwidgets_INCLUDE_DIRS_RELEASE} )
set(wxWidgets_INCLUDE_DIR ${wxwidgets_INCLUDE_DIRS_RELEASE} )
set(wxWidgets_LIBRARIES ${wxwidgets_LIBRARIES_RELEASE} )
set(wxWidgets_DEFINITIONS ${wxwidgets_DEFINITIONS_RELEASE} )


# Definition of extra CMake variables from cmake_extra_variables


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${wxwidgets_BUILD_MODULES_PATHS_RELEASE} )
    message(${wxWidgets_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


