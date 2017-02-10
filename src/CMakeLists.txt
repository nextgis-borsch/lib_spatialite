################################################################################
# Project:  Lib spatialite
# Purpose:  CMake build scripts
# Author:   Dmitry Baryshnikov, <dmitry.baryshnikov@nextgis.com>
################################################################################
# Copyright (C) 2017, NextGIS <info@nextgis.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
################################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)

set(TARGET_OBJECTS)

add_subdirectory(headers)
add_subdirectory(gaiaaux)
add_subdirectory(gaiaexif)
add_subdirectory(gaiageo)
add_subdirectory(spatialite)
add_subdirectory(shapefiles)
add_subdirectory(srsinit)
add_subdirectory(connection_cache)
add_subdirectory(virtualtext)
add_subdirectory(wfs)
add_subdirectory(dxf)
add_subdirectory(md5)

if(ENABLE_GCP)
    add_subdirectory(control_points)
endif()
if(ENABLE_GEOPACKAGE)
    add_subdirectory(geopackage)
endif()

set(CSOURCES
    versioninfo/version.c
)

if(BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${CSOURCES} ${TARGET_OBJECTS} ${PRIV_HEADERS} ${MAIN_HEADERS})
    set_target_properties (${LIB_NAME} PROPERTIES
        VERSION ${VERSION}
        SOVERSION ${SPL_MAJOR_VERSION}
        DEFINE_SYMBOL DLL_EXPORTS)
elseif(OSX_FRAMEWORK)
    add_library(${LIB_NAME} SHARED ${CSOURCES} ${TARGET_OBJECTS} ${PRIV_HEADERS} ${MAIN_HEADERS})
    set_source_files_properties("${SPL_HEADERS}" PROPERTIES MACOSX_PACKAGE_LOCATION "Headers/spatialite")
    set_target_properties(${LIB_NAME} PROPERTIES
      FRAMEWORK TRUE
      FRAMEWORK_VERSION ${FRAMEWORK_VERSION}
      MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${VERSION}
      MACOSX_FRAMEWORK_BUNDLE_VERSION ${VERSION}
      MACOSX_FRAMEWORK_IDENTIFIER org.spatialite.spatialite
      XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
      PUBLIC_HEADER "${MAIN_HEADERS}"
      VERSION ${VERSION}
      SOVERSION ${SPL_MAJOR_VERSION})
else()
    add_library(${LIB_NAME} STATIC ${CSOURCES} ${TARGET_OBJECTS} ${PRIV_HEADERS} ${MAIN_HEADERS})
endif()

target_link_extlibraries(${LIB_NAME})

#------------------------------------------------------------------------------
# targets installation
#------------------------------------------------------------------------------

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
    install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT libraries
        LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT libraries
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT libraries
        FRAMEWORK DESTINATION ${INSTALL_LIB_DIR}
    )
endif()
