#

#set_and_check (CFITSIO_INCLUDE_DIRS "")

#include ("${CMAKE_CURRENT_LIST_DIR}/cfitsioTargets.cmake")

# - Config file for the MyLib package
# It defines the following variables
#  MYLIB_INCLUDE_DIRS - include directories for MyLib
#  MYLIB_LIBRARIES    - libraries to link against
#  MYLIB_EXECUTABLE   - the bar executable

# Compute paths
#get_filename_component(CFITSIO_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
#set(CFITSIO_INCLUDE_DIRS "")

# These are IMPORTED targets created by ${LIB_NAME}Targets.cmake
#set(CFITSIO_LIBRARIES ${LIB_NAME})

# Our library dependencies (contains definitions for IMPORTED targets)
#if(NOT TARGET ${LIB_NAME} AND NOT CFITSIO_BINARY_DIR)
#  include("${CFITSIO_CMAKE_DIR}/cfitsio-targets.cmake")
#endif()

# Compute installation prefix relative to this file.
get_filename_component(_dir "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_prefix "${_dir}/../.." ABSOLUTE)

# Import the targets.
include("${_prefix}/lib/cfitsio-/cfitsio-targets.cmake")

# Report other information.
set(cfitsio_INCLUDE_DIRS "${_prefix}/include/cfitsio-")

# Our library dependencies (contains definitions for IMPORTED targets)
if(NOT TARGET ${LIB_NAME}::${LIB_NAME})
  include("${CFITSIO_CMAKE_DIR}/cfitsio-targets.cmake")
endif()
