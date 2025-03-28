set(_harfbuzz_libdir "/data/mxe/usr/x86_64-w64-mingw32.shared.posix.all/lib")
set(_harfbuzz_includedir "/data/mxe/usr/x86_64-w64-mingw32.shared.posix.all/include")

# Extract version information from libtool.
set(_harfbuzz_version_info "60710:0:60710")
string(REPLACE ":" ";" _harfbuzz_version_info "${_harfbuzz_version_info}")
list(GET _harfbuzz_version_info 0
  _harfbuzz_current)
list(GET _harfbuzz_version_info 1
  _harfbuzz_revision)
list(GET _harfbuzz_version_info 2
  _harfbuzz_age)
unset(_harfbuzz_version_info)

if (APPLE)
  set(_harfbuzz_lib_suffix ".0${CMAKE_SHARED_LIBRARY_SUFFIX}")
elseif (UNIX)
  set(_harfbuzz_lib_suffix "${CMAKE_SHARED_LIBRARY_SUFFIX}.0.${_harfbuzz_current}.${_harfbuzz_revision}")
else ()
  # Unsupported.
  set(harfbuzz_FOUND 0)
endif ()

# Add the libraries.
add_library(harfbuzz::harfbuzz SHARED IMPORTED)
set_target_properties(harfbuzz::harfbuzz PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_harfbuzz_includedir}/harfbuzz"
  IMPORTED_LOCATION "${_harfbuzz_libdir}/libharfbuzz${_harfbuzz_lib_suffix}")

add_library(harfbuzz::icu SHARED IMPORTED)
set_target_properties(harfbuzz::icu PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_harfbuzz_includedir}/harfbuzz"
  INTERFACE_LINK_LIBRARIES "harfbuzz::harfbuzz"
  IMPORTED_LOCATION "${_harfbuzz_libdir}/libharfbuzz-icu${_harfbuzz_lib_suffix}")

add_library(harfbuzz::subset SHARED IMPORTED)
set_target_properties(harfbuzz::subset PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_harfbuzz_includedir}/harfbuzz"
  INTERFACE_LINK_LIBRARIES "harfbuzz::harfbuzz"
  IMPORTED_LOCATION "${_harfbuzz_libdir}/libharfbuzz-subset${_harfbuzz_lib_suffix}")

# Only add the gobject library if it was built.
set(_harfbuzz_have_gobject "true")
if (_harfbuzz_have_gobject)
  add_library(harfbuzz::gobject SHARED IMPORTED)
  set_target_properties(harfbuzz::gobject PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_harfbuzz_includedir}/harfbuzz"
    INTERFACE_LINK_LIBRARIES "harfbuzz::harfbuzz"
    IMPORTED_LOCATION "${_harfbuzz_libdir}/libharfbuzz-gobject${_harfbuzz_lib_suffix}")
endif ()

# Clean out variables we used in our scope.
unset(_harfbuzz_lib_suffix)
unset(_harfbuzz_current)
unset(_harfbuzz_revision)
unset(_harfbuzz_age)
unset(_harfbuzz_includedir)
unset(_harfbuzz_libdir)
