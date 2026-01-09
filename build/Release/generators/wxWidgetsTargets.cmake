# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/wxWidgets-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${wxwidgets_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${wxWidgets_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET wxWidgets::wxWidgets)
    add_library(wxWidgets::wxWidgets INTERFACE IMPORTED)
    message(${wxWidgets_MESSAGE_MODE} "Conan: Target declared 'wxWidgets::wxWidgets'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/wxWidgets-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()