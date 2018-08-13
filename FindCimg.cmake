find_path(CIMG_INCLUDE_DIR
        CImg.h
        HINTS ${CIMG_ROOT_DIR} ${CIMG_ROOT_DIR}/include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CIMG DEFAULT_MSG CIMG_INCLUDE_DIR)

if(CIMG_FOUND)
    mark_as_advanced(CIMG_INCLUDE_DIR)
    set(CIMG_INCLUDE_DIRS ${CIMG_INCLUDE_DIR})
endif()