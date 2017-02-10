################################################################################
# Project:  Lib spatialite
# Purpose:  CMake build scripts
# Author:   Alexander Lisovenko, alexander.lisovenko@nexgis.com
# Author:   Dmitry Baryshnikov, <dmitry.baryshnikov@nextgis.com>
################################################################################
# Copyright (C) 2016-2017, NextGIS <info@nextgis.com>
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

cmake_minimum_required(VERSION 2.8.12)
project(spatialite)

#------------------------------------------------------------------------------
# internal cmake settings
#------------------------------------------------------------------------------
set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(OSX_FRAMEWORK AND (BUILD_SHARED_LIBS OR BUILD_STATIC_LIBS))
  message(FATAL_ERROR "Only OSX_FRAMEWORK key or any or both BUILD_SHARED_LIBS
                       and BUILD_STATIC_LIBS keys are permitted")
endif()

if(OSX_FRAMEWORK)
  set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/Applications" CACHE INTERNAL "Installation directory for executables" FORCE)
  set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/Library/Frameworks" CACHE INTERNAL "Installation directory for libraries" FORCE)
  set(INSTALL_INC_DIR "${INSTALL_LIB_DIR}/${PROJECT_NAME}.framework/Headers" CACHE INTERNAL "Installation directory for headers" FORCE)
#  set(SKIP_INSTALL_HEADERS ON)
  set(SKIP_INSTALL_EXECUTABLES ON)
  set(SKIP_INSTALL_FILES ON)
  set(SKIP_INSTALL_EXPORT ON)
  set(CMAKE_MACOSX_RPATH ON)
else()
    include(GNUInstallDirs)
    set(INSTALL_BIN_DIR ${CMAKE_INSTALL_FULL_BINDIR} CACHE INTERNAL "Installation directory for executables" FORCE)
    set(INSTALL_LIB_DIR ${CMAKE_INSTALL_FULL_LIBDIR} CACHE INTERNAL "Installation directory for libraries" FORCE)
    set(INSTALL_INC_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}/${PROJECT_NAME}" CACHE INTERNAL "Installation directory for headers" FORCE)
endif()

include(util)
check_version(SPL_MAJOR_VERSION SPL_MINOR_VERSION SPL_REV_VERSION)
set(VERSION ${SPL_MAJOR_VERSION}.${SPL_MINOR_VERSION}.${SPL_REV_VERSION})

if(OSX_FRAMEWORK)
    set(FRAMEWORK_VERSION "${SPL_MAJOR_VERSION}")
endif()
report_version(${PROJECT_NAME} ${VERSION})

#------------------------------------------------------------------------------
# search for dependencies
#------------------------------------------------------------------------------
include(FindAnyProject)

find_anyproject(Sqlite3 REQUIRED)
if(SQLITE3_FOUND)
    set(HAVE_LIBSQLITE3 ON)
endif()
find_anyproject(ZLIB REQUIRED)
if(ZLIB_FOUND)
    set(HAVE_LIBZ ON)
endif()

option(OMIT_GEOS "Should be defined in order to disable GEOS support" OFF)
if(NOT OMIT_GEOS)
    option(GEOS_ADVANCED "Should be defined in order to enable GEOS_ADVANCED support" ON)
    option(GEOS_TRUNK "Should be defined in order to enable GEOS_TRUNK experimental support" OFF)
    if(GEOS_ADVANCED)
        find_anyproject(GEOS VERSION 3.4.0 REQUIRED)
    else()
        find_anyproject(GEOS VERSION 3.3.0 REQUIRED)
    endif()
endif()

option(OMIT_PROJ "Should be defined in order to disable PROJ.4 support" OFF)
if(NOT OMIT_PROJ)
    find_anyproject(PROJ4 REQUIRED)
endif()
option(OMIT_ICONV "Should be defined in order to disable ICONV support" OFF)
if(NOT OMIT_ICONV)
    find_anyproject(ICONV REQUIRED)
endif()
option(OMIT_MATHSQL "Should be defined in order to disable MATHSQL support" OFF)
option(OMIT_EPSG "Should be defined in order to disable EPSG full support" OFF)
option(OMIT_GEOCALLBACKS "Should be defined in order to disable GEOCALLBACKS support" OFF)
option(OMIT_FREEXL "Should be defined in order to disable FREEXL support" OFF)
if(NOT OMIT_FREEXL)
    find_anyproject(Freexl REQUIRED)
endif()
option(ENABLE_LIBXML2 "Should be defined in order to enable LIBXML2 support" ON)
if(ENABLE_LIBXML2)
    find_anyproject(LibXml2 REQUIRED)
endif()

if(NOT MSVC)
    option(ENABLE_GCP "Should be defined in order to enable GCP support" ON)
    option(ENABLE_GEOPACKAGE "Should be defined in order to enable GeoPackage support" ON)
endif()

option(ENABLE_LWGEOM "Should be defined in order to enable LWGEOM support" ON)
if(ENABLE_LWGEOM)
    set(POSTGIS_2_1 ON)
    # TODO find_anyproject(LWGEOM DEFAULT ON)
endif()

include(configure)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(LIB_NAME ${PROJECT_NAME})

add_subdirectory(src)

if (REGISTER_PACKAGE)
    export(TARGETS ${LIB_NAME} FILE ${LIB_NAME}-exports.cmake)

    set(EXPORT_PACKAGE_NAME Spatialite)
    string(TOUPPER ${EXPORT_PACKAGE_NAME} EXPORT_PACKAGE_NAME_UPPER)
    set(EXPORT_PACKAGE_LIB_NAME ${LIB_NAME})

    export(PACKAGE ${EXPORT_PACKAGE_NAME})

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/PackageConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_PACKAGE_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_PACKAGE_NAME}ConfigVersion.cmake
        VERSION ${VERSION}
        COMPATIBILITY AnyNewerVersion )
else()
    export(TARGETS ${LIB_NAME} FILE ${LIB_NAME}-exports.cmake EXPORT_LINK_INTERFACE_LIBRARIES)
endif()

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
