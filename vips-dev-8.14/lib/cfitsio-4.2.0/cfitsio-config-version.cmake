set(PACKAGE_VERSION "4.2.0")

# Check whether the requested PACKAGE_FIND_VERSION is compatible
if("${PACKAGE_VERSION}" VERSION_LESS "${PACKAGE_FIND_VERSION}")
  set(PACKAGE_VERSION_COMPATIBLE FALSE)
else()
  set(PACKAGE_VERSION_COMPATIBLE TRUE)
  if ("${PACKAGE_VERSION}" VERSION_EQUAL "${PACKAGE_FIND_VERSION}")
    set(PACKAGE_VERSION_EXACT TRUE)
  endif()
endif()

# alternate:
#set(PACKAGE_VERSION "")
#if(NOT "${PACKAGE_FIND_VERSION}" VERSION_GREATER "")
#  set(PACKAGE_VERSION_COMPATIBLE 1) # compatible with older
#  if("${PACKAGE_FIND_VERSION}" VERSION_EQUAL "")
#    set(PACKAGE_VERSION_EXACT 1) # exact match for this version
#  endif()
#endif()
