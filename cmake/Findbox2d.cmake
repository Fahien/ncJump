# - Find Box2D library
# Find the native Box2D headers and libraries.
#
#  BOX2D_INCLUDE_DIRS - where to find box2d/box2d.h, etc.
#  BOX2D_LIBRARIES    - List of libraries when using box2d.
#  BOX2D_FOUND        - True if box2d is found.

# Look for the header file.
FIND_PATH(BOX2D_INCLUDE_DIR NAMES box2d/box2d.h)
MARK_AS_ADVANCED(BOX2D_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(BOX2D_LIBRARY NAMES box2d)
MARK_AS_ADVANCED(BOX2D_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set WEBFOUND_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(box2d DEFAULT_MSG BOX2D_LIBRARY BOX2D_INCLUDE_DIR)

SET(BOX2D_LIBRARIES ${BOX2D_LIBRARY})
SET(BOX2D_INCLUDE_DIRS ${BOX2D_INCLUDE_DIR})
