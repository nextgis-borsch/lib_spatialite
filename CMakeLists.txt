################################################################################
# Project:  Lib spatialite
# Purpose:  CMake build scripts
# Author:   Alexander Lisovenko, alexander.lisovenko@nexgis.com
# Author:   Dmitry Baryshnikov, <dmitry.baryshnikov@nextgis.com>
################################################################################
# Copyright (C) 2016-2019, NextGIS <info@nextgis.com>
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

if(NOT DEFINED PACKAGE_VENDOR)
    set(PACKAGE_VENDOR NextGIS)
endif()

if(NOT DEFINED PACKAGE_BUGREPORT)
    set(PACKAGE_BUGREPORT info@nextgis.com)
endif()

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
    set(INSTALL_BIN_DIR "bin" CACHE INTERNAL "Installation directory for executables" FORCE)
    set(INSTALL_LIB_DIR "Library/Frameworks" CACHE INTERNAL "Installation directory for libraries" FORCE)
    set(INSTALL_INC_DIR "${INSTALL_LIB_DIR}/${PROJECT_NAME}.framework/Headers" CACHE INTERNAL "Installation directory for headers" FORCE)
    set(INSTALL_CMAKECONF_DIR ${INSTALL_LIB_DIR}/${PROJECT_NAME}.framework/Resources/CMake CACHE INTERNAL "Installation directory for cmake config files" FORCE)
    #  set(SKIP_INSTALL_HEADERS ON)
    set(SKIP_INSTALL_EXECUTABLES ON)
    set(SKIP_INSTALL_FILES ON)
    set(SKIP_INSTALL_EXPORT ON)
    set(CMAKE_MACOSX_RPATH ON)
else()
    include(GNUInstallDirs)
    set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "Installation directory for executables" FORCE)
    set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "Installation directory for libraries" FORCE)
    set(INSTALL_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE INTERNAL "Installation directory for headers" FORCE)
    set(INSTALL_CMAKECONF_DIR ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/CMake CACHE INTERNAL "Installation directory for cmake config files" FORCE)
endif()

include(util)
check_version(SPL_MAJOR_VERSION SPL_MINOR_VERSION SPL_REV_VERSION)
set(VERSION ${SPL_MAJOR_VERSION}.${SPL_MINOR_VERSION}.${SPL_REV_VERSION})

if(OSX_FRAMEWORK)
    set(FRAMEWORK_VERSION "${SPL_MAJOR_VERSION}")
endif()
report_version(${PROJECT_NAME} ${VERSION})

if(CMAKE_GENERATOR_TOOLSET MATCHES "v([0-9]+)_xp")
    add_definitions(-D_WIN32_WINNT=0x0501)
endif()
#------------------------------------------------------------------------------
# search for dependencies
#------------------------------------------------------------------------------
include(FindAnyProject)

find_anyproject(SQLite3 REQUIRED)
if(SQLITE3_FOUND)
    set(HAVE_LIBSQLITE3 ON)
    set(HAVE_SQLITE3_H ON)
endif()
find_anyproject(ZLIB REQUIRED)
if(ZLIB_FOUND)
    set(HAVE_LIBZ ON)
    set(HAVE_ZLIB_H ON)
endif()

option(OMIT_GEOS "Should be defined in order to disable GEOS support" OFF)
if(NOT OMIT_GEOS)
    option(GEOS_ADVANCED "Should be defined in order to enable GEOS_ADVANCED support" ON)
    if(GEOS_ADVANCED)
        find_anyproject(GEOS VERSION 3.4.0 REQUIRED)
    else()
        find_anyproject(GEOS VERSION 3.3.0 REQUIRED)
    endif()
    option(GEOS_REENTRANT "Should be defined in order to enable GEOS_REENTRANT (fully thread-safe)" ON)
endif()

if(GEOS_FOUND)
    set(HAVE_GEOS_C_H ON)
    if(GEOS_VERSION VERSION_GREATER_EQUAL "3.7.0")
        set(GEOS_370 ON)
    endif()
endif()

option(OMIT_PROJ "Should be defined in order to disable PROJ support" OFF)
if(NOT OMIT_PROJ)
    find_anyproject(PROJ REQUIRED)
endif()
if(PROJ_FOUND)
    set(HAVE_PROJ_H 1)
    add_definitions(-DPROJ_MSVC_DLL_IMPORT)
    if(PROJ_VERSION VERSION_GREATER_EQUAL "6.0.0")
        set(PROJ_NEW ON)
    endif()
endif()
option(OMIT_ICONV "Should be defined in order to disable ICONV support" OFF)
if(NOT OMIT_ICONV)
    find_anyproject(ICONV REQUIRED)
endif()

if(ICONV_FOUND)
    set(HAVE_ICONV_H ON)
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

option(ENABLE_LWGEOM "Should be defined in order to enable LWGEOM support" OFF)
if(ENABLE_LWGEOM)
    set(POSTGIS_2_1 ON)
    # TODO find_anyproject(LWGEOM DEFAULT ON)
    if(LWGEOM_FOUND)
        set(HAVE_LIBLWGEOM_H ON)
    endif()
endif()

option(ENABLE_RTTOPO "Should be defined in order to enable RTTOPO support." OFF)

include(configure)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(LIB_NAME ${PROJECT_NAME})
set(PACKAGE_NAME ${PROJECT_NAME})
string(TOUPPER ${PACKAGE_NAME} PACKAGE_UPPER_NAME)

add_subdirectory(src)
if(BUILD_QT4)
    add_subdirectory(qt4)
endif()

enable_testing()
add_subdirectory(test)

# uninstall
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# Export package ===============================================================

if(REGISTER_PACKAGE)
    # Export the package for use from the build-tree
    # (this registers the build-tree with a global CMake-registry)
    export(PACKAGE ${PACKAGE_UPPER_NAME})
endif()

# Archiving ====================================================================

set(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR "${PACKAGE_VENDOR}")
set(CPACK_PACKAGE_VERSION "${VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE_NAME} Installation")
set(CPACK_PACKAGE_RELOCATABLE TRUE)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_GENERATOR "ZIP")
set(CPACK_MONOLITHIC_INSTALL ON)
set(CPACK_STRIP_FILES TRUE)

# Get cpack zip archive name
get_cpack_filename(${VERSION} PROJECT_CPACK_FILENAME)
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_CPACK_FILENAME})

include(CPack)
