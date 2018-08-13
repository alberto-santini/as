find_path(CONCORDE_INCLUDE_DIR
        concorde.h
        HINTS ${CONCORDE_ROOT_DIR} ${CONCORDE_ROOT_DIR}/include)

find_library(CONCORDE_LIB NAMES concorde libconcorde
        HINTS ${CONCORDE_ROOT_DIR} ${CONCORDE_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CONCORDE DEFAULT_MSG CONCORDE_LIB CONCORDE_INCLUDE_DIR)

if(CONCORDE_FOUND)
    mark_as_advanced(CONCORDE_INCLUDE_DIR)
    set(CONCORDE_INCLUDE_DIRS ${CONCORDE_INCLUDE_DIR})
    set(CONCORDE_LIBRARIES ${CONCORDE_LIB})
endif()