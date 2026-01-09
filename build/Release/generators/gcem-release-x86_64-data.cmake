########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(gcem_COMPONENT_NAMES "")
if(DEFINED gcem_FIND_DEPENDENCY_NAMES)
  list(APPEND gcem_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES gcem_FIND_DEPENDENCY_NAMES)
else()
  set(gcem_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(gcem_PACKAGE_FOLDER_RELEASE "/home/azusa/.conan2/p/gcema9dedeb3348fa/p")
set(gcem_BUILD_MODULES_PATHS_RELEASE )


set(gcem_INCLUDE_DIRS_RELEASE "${gcem_PACKAGE_FOLDER_RELEASE}/include")
set(gcem_RES_DIRS_RELEASE )
set(gcem_DEFINITIONS_RELEASE )
set(gcem_SHARED_LINK_FLAGS_RELEASE )
set(gcem_EXE_LINK_FLAGS_RELEASE )
set(gcem_OBJECTS_RELEASE )
set(gcem_COMPILE_DEFINITIONS_RELEASE )
set(gcem_COMPILE_OPTIONS_C_RELEASE )
set(gcem_COMPILE_OPTIONS_CXX_RELEASE )
set(gcem_LIB_DIRS_RELEASE )
set(gcem_BIN_DIRS_RELEASE )
set(gcem_LIBRARY_TYPE_RELEASE UNKNOWN)
set(gcem_IS_HOST_WINDOWS_RELEASE 0)
set(gcem_LIBS_RELEASE )
set(gcem_SYSTEM_LIBS_RELEASE )
set(gcem_FRAMEWORK_DIRS_RELEASE )
set(gcem_FRAMEWORKS_RELEASE )
set(gcem_BUILD_DIRS_RELEASE )
set(gcem_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(gcem_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${gcem_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${gcem_COMPILE_OPTIONS_C_RELEASE}>")
set(gcem_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${gcem_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${gcem_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${gcem_EXE_LINK_FLAGS_RELEASE}>")


set(gcem_COMPONENTS_RELEASE )