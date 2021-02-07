#[=======================================================================[.rst:
Findbox2d
--------

Find the Box2D library (``libbox2d``)

Imported targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` targets:

``box2d::box2d``
  The Box2D library, if found.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``BOX2D_FOUND``
  If false, do not try to use Box2D.
``BOX2D_INCLUDE_DIRS``
  where to find box2d.h, etc.
``BOX2D_LIBRARIES``
  the libraries needed to use Box2D.
``BOX2D_VERSION``
  the version of the Box2D library found

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``BOX2D_INCLUDE_DIRS``
  where to find box2d.h, etc.
``BOX2D_LIBRARY_RELEASE``
  where to find the Box2D library (optimized).
``BOX2D_LIBRARY_DEBUG``
  where to find the Box2D library (debug).

Obsolete variables
^^^^^^^^^^^^^^^^^^

``BOX2D_INCLUDE_DIR``
  where to find box2d.h, etc. (same as BOX2D_INCLUDE_DIRS)
``BOX2D_LIBRARY``
  where to find the Box2D library.
#]=======================================================================]

find_path(BOX2D_INCLUDE_DIR NAMES box2d/box2d.h)

set(box2d_names ${BOX2D_NAMES} box2d box2d-static libbox2d libbox2d-static)
foreach(name ${box2d_names})
  list(APPEND box2d_names_debug "${name}d")
endforeach()

if(NOT BOX2D_LIBRARY)
  find_library(BOX2D_LIBRARY_RELEASE NAMES ${box2d_names} NAMES_PER_DIR)
  find_library(BOX2D_LIBRARY_DEBUG NAMES ${box2d_names_debug} NAMES_PER_DIR)
  include(${CMAKE_ROOT}/Modules/SelectLibraryConfigurations.cmake)
  select_library_configurations(BOX2D)
  mark_as_advanced(BOX2D_LIBRARY_RELEASE BOX2D_LIBRARY_DEBUG)
endif()
unset(box2d_names)
unset(box2d_names_debug)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(box2d
  REQUIRED_VARS BOX2D_LIBRARY BOX2D_INCLUDE_DIR)

if(BOX2D_FOUND)
  set(BOX2D_LIBRARIES ${BOX2D_LIBRARY})
  set(BOX2D_INCLUDE_DIRS ${BOX2D_INCLUDE_DIR})

  if(NOT TARGET box2d::box2d)
    add_library(box2d::box2d UNKNOWN IMPORTED)
    if(BOX2D_INCLUDE_DIRS)
      set_target_properties(box2d::box2d PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${BOX2D_INCLUDE_DIRS}")
    endif()
    if(EXISTS "${BOX2D_LIBRARY}")
      set_target_properties(box2d::box2d PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${BOX2D_LIBRARY}")
    endif()
    if(EXISTS "${BOX2D_LIBRARY_RELEASE}")
      set_property(TARGET box2d::box2d APPEND PROPERTY
        IMPORTED_CONFIGURATIONS RELEASE)
      set_target_properties(box2d::box2d PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
        IMPORTED_LOCATION_RELEASE "${BOX2D_LIBRARY_RELEASE}")
    endif()
    if(EXISTS "${BOX2D_LIBRARY_DEBUG}")
      set_property(TARGET box2d::box2d APPEND PROPERTY
        IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties(box2d::box2d PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
        IMPORTED_LOCATION_DEBUG "${BOX2D_LIBRARY_DEBUG}")
    endif()
  endif()
endif()

mark_as_advanced(BOX2D_LIBRARY BOX2D_INCLUDE_DIR)
