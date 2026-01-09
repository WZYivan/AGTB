########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(wxwidgets_COMPONENT_NAMES "")
if(DEFINED wxwidgets_FIND_DEPENDENCY_NAMES)
  list(APPEND wxwidgets_FIND_DEPENDENCY_NAMES gtk opengl_system xkbcommon xorg CURL PNG TIFF JPEG EXPAT PCRE2 ZLIB)
  list(REMOVE_DUPLICATES wxwidgets_FIND_DEPENDENCY_NAMES)
else()
  set(wxwidgets_FIND_DEPENDENCY_NAMES gtk opengl_system xkbcommon xorg CURL PNG TIFF JPEG EXPAT PCRE2 ZLIB)
endif()
set(gtk_FIND_MODE "NO_MODULE")
set(opengl_system_FIND_MODE "NO_MODULE")
set(xkbcommon_FIND_MODE "NO_MODULE")
set(xorg_FIND_MODE "NO_MODULE")
set(CURL_FIND_MODE "MODULE")
set(PNG_FIND_MODE "MODULE")
set(TIFF_FIND_MODE "MODULE")
set(JPEG_FIND_MODE "MODULE")
set(EXPAT_FIND_MODE "MODULE")
set(PCRE2_FIND_MODE "NO_MODULE")
set(ZLIB_FIND_MODE "MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(wxwidgets_PACKAGE_FOLDER_RELEASE "/home/azusa/.conan2/p/b/wxwid53040702a620d/p")
set(wxwidgets_BUILD_MODULES_PATHS_RELEASE )


set(wxwidgets_INCLUDE_DIRS_RELEASE "${wxwidgets_PACKAGE_FOLDER_RELEASE}/include/wx-3.2"
			"${wxwidgets_PACKAGE_FOLDER_RELEASE}/include")
set(wxwidgets_RES_DIRS_RELEASE )
set(wxwidgets_DEFINITIONS_RELEASE "-DwxUSE_GUI=1"
			"-D__WXGTK__")
set(wxwidgets_SHARED_LINK_FLAGS_RELEASE )
set(wxwidgets_EXE_LINK_FLAGS_RELEASE )
set(wxwidgets_OBJECTS_RELEASE )
set(wxwidgets_COMPILE_DEFINITIONS_RELEASE "wxUSE_GUI=1"
			"__WXGTK__")
set(wxwidgets_COMPILE_OPTIONS_C_RELEASE )
set(wxwidgets_COMPILE_OPTIONS_CXX_RELEASE )
set(wxwidgets_LIB_DIRS_RELEASE "${wxwidgets_PACKAGE_FOLDER_RELEASE}/lib")
set(wxwidgets_BIN_DIRS_RELEASE )
set(wxwidgets_LIBRARY_TYPE_RELEASE STATIC)
set(wxwidgets_IS_HOST_WINDOWS_RELEASE 0)
set(wxwidgets_LIBS_RELEASE wx_gtk2u_xrc-3.2 wx_gtk2u_stc-3.2 wxscintilla-3.2 wx_gtk2u_richtext-3.2 wx_gtk2u_ribbon-3.2 wx_gtk2u_qa-3.2 wx_gtk2u_propgrid-3.2 wx_gtk2u_html-3.2 wx_gtk2u_gl-3.2 wx_gtk2u_aui-3.2 wx_baseu_xml-3.2 wx_baseu_net-3.2 wx_gtk2u_adv-3.2 wx_gtk2u_core-3.2 wx_baseu-3.2)
set(wxwidgets_SYSTEM_LIBS_RELEASE dl pthread SM)
set(wxwidgets_FRAMEWORK_DIRS_RELEASE )
set(wxwidgets_FRAMEWORKS_RELEASE )
set(wxwidgets_BUILD_DIRS_RELEASE )
set(wxwidgets_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(wxwidgets_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${wxwidgets_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${wxwidgets_COMPILE_OPTIONS_C_RELEASE}>")
set(wxwidgets_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${wxwidgets_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${wxwidgets_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${wxwidgets_EXE_LINK_FLAGS_RELEASE}>")


set(wxwidgets_COMPONENTS_RELEASE )