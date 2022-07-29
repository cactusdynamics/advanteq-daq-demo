include(FindPackageHandleStandardArgs)

find_library(DAQNAVI_LIBRARY
  NAMES
  biodaq
)

find_path(DAQNAVI_INCLUDE_DIR
  NAMES
  bdaqctrl.h
  PATHS
  /opt/advantech/inc
)

find_package_handle_standard_args(biodaq REQUIRED_VARS DAQNAVI_LIBRARY DAQNAVI_INCLUDE_DIR)

if (biodaq_FOUND)
  mark_as_advanced(DAQNAVI_LIBRARY)
  mark_as_advanced(DAQNAVI_INCLUDE_DIR)
endif()

if (biodaq_FOUND AND NOT TARGET biodaq)
  add_library(biodaq SHARED IMPORTED)
  set_property(TARGET biodaq PROPERTY IMPORTED_LOCATION ${DAQNAVI_LIBRARY})
  target_include_directories(biodaq INTERFACE ${DAQNAVI_INCLUDE_DIR})
endif()
