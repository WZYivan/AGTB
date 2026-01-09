# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(gcem_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(gcem_FRAMEWORKS_FOUND_RELEASE "${gcem_FRAMEWORKS_RELEASE}" "${gcem_FRAMEWORK_DIRS_RELEASE}")

set(gcem_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET gcem_DEPS_TARGET)
    add_library(gcem_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET gcem_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${gcem_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${gcem_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### gcem_DEPS_TARGET to all of them
conan_package_library_targets("${gcem_LIBS_RELEASE}"    # libraries
                              "${gcem_LIB_DIRS_RELEASE}" # package_libdir
                              "${gcem_BIN_DIRS_RELEASE}" # package_bindir
                              "${gcem_LIBRARY_TYPE_RELEASE}"
                              "${gcem_IS_HOST_WINDOWS_RELEASE}"
                              gcem_DEPS_TARGET
                              gcem_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "gcem"    # package_name
                              "${gcem_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${gcem_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET gcem
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${gcem_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${gcem_LIBRARIES_TARGETS}>
                 )

    if("${gcem_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET gcem
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     gcem_DEPS_TARGET)
    endif()

    set_property(TARGET gcem
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${gcem_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET gcem
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${gcem_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET gcem
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${gcem_LIB_DIRS_RELEASE}>)
    set_property(TARGET gcem
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${gcem_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET gcem
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${gcem_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(gcem_LIBRARIES_RELEASE gcem)
