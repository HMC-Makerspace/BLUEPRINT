#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mozjpeg::jpeg" for configuration "Release"
set_property(TARGET mozjpeg::jpeg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mozjpeg::jpeg PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libjpeg.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libjpeg-62.dll"
  )

list(APPEND _cmake_import_check_targets mozjpeg::jpeg )
list(APPEND _cmake_import_check_files_for_mozjpeg::jpeg "${_IMPORT_PREFIX}/lib/libjpeg.dll.a" "${_IMPORT_PREFIX}/bin/libjpeg-62.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
