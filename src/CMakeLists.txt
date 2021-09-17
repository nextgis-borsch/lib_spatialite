################################################################################
# Project:  Lib spatialite
# Purpose:  CMake build scripts
# Author:   Dmitry Baryshnikov, <dmitry.baryshnikov@nextgis.com>
################################################################################
# Copyright (C) 2017-2019, NextGIS <info@nextgis.com>
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

if(MSVC AND BUILD_SHARED_LIBS)
    add_definitions(-DDLL_EXPORT)
endif()

add_subdirectory(headers)
add_subdirectory(gaiaaux)
add_subdirectory(gaiaexif)
add_subdirectory(gaiageo)
add_subdirectory(spatialite)
add_subdirectory(shapefiles)
add_subdirectory(srsinit)
add_subdirectory(connection_cache)
# add_subdirectory(virtualtext)
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
    virtualtext/virtualtext.c
    cutter/gaia_cutter.c
    stored_procedures/stored_procedures.c
)

if(ENABLE_RTTOPO)
    add_subdirectory(topology)
else()
    set(CSOURCES ${CSOURCES} topology/gaia_auxtopo.c)
endif()

if(BUILD_SHARED_LIBS)
    add_library(${LIB_NAME} SHARED ${CSOURCES} ${TARGET_OBJECTS} ${PRIV_HEADERS} ${MAIN_HEADERS})
    set_target_properties (${LIB_NAME} PROPERTIES
        VERSION ${VERSION}
        SOVERSION ${SPL_MAJOR_VERSION})
    if(MSVC)
        set_target_properties (${LIB_NAME} PROPERTIES
            DEFINE_SYMBOL DLL_EXPORT)
    endif()
elseif(OSX_FRAMEWORK)
    # set(SPL_HEADERS ${SPL_HEADERS}
    #     ${CMAKE_CURRENT_BINARY_DIR}/spatialite/gaiaconfig.h
    # )
    add_library(${LIB_NAME} SHARED ${CSOURCES} ${TARGET_OBJECTS} ${PRIV_HEADERS} ${MAIN_HEADERS} ${SPL_HEADERS} ${GAIACONFIG_HEADERS})
    set_property(SOURCE ${SPL_HEADERS} PROPERTY MACOSX_PACKAGE_LOCATION "Headers/spatialite")
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

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS ${LIB_NAME}
        EXPORT ${PACKAGE_UPPER_NAME}Targets
        RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT libraries
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT libraries
        LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT libraries
        INCLUDES DESTINATION ${INSTALL_INC_DIR}
        FRAMEWORK DESTINATION ${INSTALL_LIB_DIR}
    )
endif()

if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)
    install(FILES ${HHEADERS} DESTINATION "${INSTALL_INC_DIR}" COMPONENT headers)
endif()

# Add path to includes to build-tree export
target_include_directories(${LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
)

# Add all targets to the build-tree export set
export(TARGETS ${LIB_NAME}
    FILE ${PROJECT_BINARY_DIR}/${PACKAGE_UPPER_NAME}Targets.cmake)

# Create the <Package>Config.cmake file
configure_file(${PROJECT_SOURCE_DIR}/cmake/PackageConfig.cmake.in
    ${PROJECT_BINARY_DIR}/${PACKAGE_UPPER_NAME}Config.cmake @ONLY)

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
    # Install the <Package>Config.cmake
    install(FILES
      ${PROJECT_BINARY_DIR}/${PACKAGE_UPPER_NAME}Config.cmake
      DESTINATION ${INSTALL_CMAKECONF_DIR} COMPONENT dev)

    # Install the export set for use with the install-tree
    install(EXPORT ${PACKAGE_UPPER_NAME}Targets DESTINATION ${INSTALL_CMAKECONF_DIR} COMPONENT dev)
endif()
