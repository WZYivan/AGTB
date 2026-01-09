# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(wxwidgets_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(wxwidgets_FRAMEWORKS_FOUND_RELEASE "${wxwidgets_FRAMEWORKS_RELEASE}" "${wxwidgets_FRAMEWORK_DIRS_RELEASE}")

set(wxwidgets_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET wxwidgets_DEPS_TARGET)
    add_library(wxwidgets_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET wxwidgets_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${wxwidgets_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${wxwidgets_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:gtk::gtk;opengl::opengl;xkbcommon::xkbcommon;xorg::xorg;CURL::libcurl;PNG::PNG;TIFF::TIFF;JPEG::JPEG;EXPAT::EXPAT;pcre2::pcre2;ZLIB::ZLIB>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### wxwidgets_DEPS_TARGET to all of them
conan_package_library_targets("${wxwidgets_LIBS_RELEASE}"    # libraries
                              "${wxwidgets_LIB_DIRS_RELEASE}" # package_libdir
                              "${wxwidgets_BIN_DIRS_RELEASE}" # package_bindir
                              "${wxwidgets_LIBRARY_TYPE_RELEASE}"
                              "${wxwidgets_IS_HOST_WINDOWS_RELEASE}"
                              wxwidgets_DEPS_TARGET
                              wxwidgets_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "wxwidgets"    # package_name
                              "${wxwidgets_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${wxwidgets_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET wxWidgets::wxWidgets
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${wxwidgets_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${wxwidgets_LIBRARIES_TARGETS}>
                 )

    if("${wxwidgets_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET wxWidgets::wxWidgets
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     wxwidgets_DEPS_TARGET)
    endif()

    set_property(TARGET wxWidgets::wxWidgets
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${wxwidgets_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET wxWidgets::wxWidgets
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${wxwidgets_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET wxWidgets::wxWidgets
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${wxwidgets_LIB_DIRS_RELEASE}>)
    set_property(TARGET wxWidgets::wxWidgets
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${wxwidgets_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET wxWidgets::wxWidgets
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${wxwidgets_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(wxwidgets_LIBRARIES_RELEASE wxWidgets::wxWidgets)
