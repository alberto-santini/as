find_path(DISCORDE_INCLUDE_DIR
        discorde/discorde_cpp.h
        HINTS ${DISCORDE_ROOT_DIR} ${DISCORDE_ROOT_DIR}/include)

find_library(DISCORDE_C_LIB NAMES discorde_c libdiscorde_c
        HINTS ${DISCORDE_ROOT_DIR} ${DISCORDE_ROOT_DIR}/lib)

find_library(DISCORDE_CPP_LIB NAMES discorde_cpp libdiscorde_cpp
        HINTS ${DISCORDE_ROOT_DIR} ${DISCORDE_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DISCORDE DEFAULT_MSG DISCORDE_C_LIB DISCORDE_CPP_LIB DISCORDE_INCLUDE_DIR)

if(DISCORDE_FOUND)
    mark_as_advanced(DISCORDE_INCLUDE_DIR DISCORDE_C_LIB DISCORDE_CPP_LIB)
    set(DISCORDE_INCLUDE_DIRS ${DISCORDE_INCLUDE_DIR})
    set(DISCORDE_LIBRARIES ${DISCORDE_CPP_LIB} ${DISCORDE_C_LIB})
endif()