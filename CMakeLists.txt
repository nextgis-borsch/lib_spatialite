################################################################################
# Project:  Lib spatialite
# Purpose:  CMake build scripts
# Author:   Alexander Lisovenko, alexander.lisovenko@nexgis.com
################################################################################
# Copyright (C) 2016, NextGIS <info@nextgis.com>
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

cmake_minimum_required(VERSION 2.6.0)
project(spatialite)

#------------------------------------------------------------------------------
# internal cmake settings
#------------------------------------------------------------------------------
set(CMAKE_COLOR_MAKEFILE ON)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(GNUInstallDirs)
set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "Installation directory for executables" FORCE)
set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "Installation directory for libraries" FORCE)
set(INSTALL_INC_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" CACHE INTERNAL "Installation directory for headers" FORCE)

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)

#------------------------------------------------------------------------------
# Source files specification
#------------------------------------------------------------------------------
set(SOURCES
    src/gaiaaux/gg_sqlaux.c
    src/gaiaaux/gg_utf8.c
    
	src/gaiaexif/gaia_exif.c
    
    src/gaiageo/gg_advanced.c
	src/gaiageo/gg_endian.c
    src/gaiageo/gg_ewkt.c
	src/gaiageo/gg_geodesic.c
    src/gaiageo/gg_geoJSON.c
	src/gaiageo/gg_geometries.c
    src/gaiageo/gg_geoscvt.c
	src/gaiageo/gg_gml.c
    src/gaiageo/gg_kml.c
	src/gaiageo/gg_relations.c
    src/gaiageo/gg_shape.c
	src/gaiageo/gg_transform.c
    src/gaiageo/gg_vanuatu.c
	src/gaiageo/gg_wkb.c
    src/gaiageo/gg_wkt.c
	src/gaiageo/gg_extras.c
    src/gaiageo/gg_xml.c
	src/gaiageo/gg_voronoj.c
    src/gaiageo/gg_matrix.c
	src/gaiageo/gg_relations_ext.c
    
    src/connection_cache/alloc_cache.c
    
    src/shapefiles/shapefiles.c
	
    src/spatialite/mbrcache.c
    src/spatialite/spatialite.c
    src/spatialite/virtualdbf.c
	src/spatialite/virtualfdo.c
    src/spatialite/virtualnetwork.c
	src/spatialite/virtualshape.c
    src/spatialite/virtualspatialindex.c
	src/spatialite/statistics.c
    src/spatialite/metatables.c
	src/spatialite/virtualXL.c
    src/spatialite/extra_tables.c
	src/spatialite/virtualxpath.c
    src/spatialite/virtualbbox.c
	src/spatialite/spatialite_init.c
    src/spatialite/se_helpers.c
	src/spatialite/srid_aux.c
    src/spatialite/table_cloner.c
	src/spatialite/virtualelementary.c
	
    src/wfs/wfs_in.c
	
    src/dxf/dxf_parser.c
    src/dxf/dxf_loader.c
    src/dxf/dxf_writer.c
	src/dxf/dxf_load_distinct.c
    src/dxf/dxf_load_mixed.c
    
	src/shapefiles/validator.c
    
    src/md5/md5.c
    src/md5/gaia_md5.c
	
    src/srsinit/srs_init.c
    src/srsinit/epsg_inlined_00.c
    src/srsinit/epsg_inlined_01.c
	src/srsinit/epsg_inlined_02.c
    src/srsinit/epsg_inlined_03.c
	src/srsinit/epsg_inlined_04.c
    src/srsinit/epsg_inlined_05.c
	src/srsinit/epsg_inlined_06.c
    src/srsinit/epsg_inlined_07.c
	src/srsinit/epsg_inlined_08.c
    src/srsinit/epsg_inlined_09.c
	src/srsinit/epsg_inlined_10.c
    src/srsinit/epsg_inlined_11.c
	src/srsinit/epsg_inlined_12.c
    src/srsinit/epsg_inlined_13.c
	src/srsinit/epsg_inlined_14.c
    src/srsinit/epsg_inlined_15.c
	src/srsinit/epsg_inlined_16.c
    src/srsinit/epsg_inlined_17.c
	src/srsinit/epsg_inlined_18.c
    src/srsinit/epsg_inlined_19.c
	src/srsinit/epsg_inlined_20.c
    src/srsinit/epsg_inlined_21.c
	src/srsinit/epsg_inlined_22.c
    src/srsinit/epsg_inlined_23.c
	src/srsinit/epsg_inlined_24.c
    src/srsinit/epsg_inlined_25.c
	src/srsinit/epsg_inlined_26.c
    src/srsinit/epsg_inlined_27.c
	src/srsinit/epsg_inlined_28.c
    src/srsinit/epsg_inlined_29.c
	src/srsinit/epsg_inlined_30.c
    src/srsinit/epsg_inlined_31.c
	src/srsinit/epsg_inlined_32.c
    src/srsinit/epsg_inlined_33.c
	src/srsinit/epsg_inlined_34.c
    src/srsinit/epsg_inlined_35.c
	src/srsinit/epsg_inlined_36.c
    src/srsinit/epsg_inlined_37.c
	src/srsinit/epsg_inlined_38.c
    src/srsinit/epsg_inlined_39.c
	src/srsinit/epsg_inlined_40.c
    src/srsinit/epsg_inlined_41.c
	src/srsinit/epsg_inlined_42.c
    src/srsinit/epsg_inlined_43.c
	src/srsinit/epsg_inlined_44.c
    src/srsinit/epsg_inlined_45.c
	src/srsinit/epsg_inlined_46.c
    src/srsinit/epsg_inlined_extra.c
	src/srsinit/epsg_inlined_prussian.c
	src/srsinit/epsg_inlined_wgs84_00.c
    src/srsinit/epsg_inlined_wgs84_01.c
	
    src/versioninfo/version.c
    
    src/virtualtext/virtualtext.c
)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src/headers")
include_directories("${PROJECT_SOURCE_DIR}/src/gaiageo")
include_directories("${PROJECT_SOURCE_DIR}/src/connection_cache")
include_directories("${PROJECT_SOURCE_DIR}/src/md5")

#------------------------------------------------------------------------------
# Platform and compiler specific settings
#------------------------------------------------------------------------------
if(WIN32)
    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_definitions(-DDLL_EXPORT)
        add_definitions(-DYY_NO_UNISTD_H)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox")
        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W3"
                CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}"
            )
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
        endif()
        
        if (MSVC12)
            add_definitions(-DRINT_PRESENT)
        endif()
        
    endif()
else()
    #TODO
endif(WIN32)

#
# Check for unistd.h
#
check_include_file(unistd.h Z_HAVE_UNISTD_H)


#------------------------------------------------------------------------------
# search for dependencies
#------------------------------------------------------------------------------
include(FindAnyProject)

find_anyproject(PROJ4 REQUIRED)
find_anyproject(GEOS REQUIRED)
find_anyproject(FREEXL REQUIRED)
find_anyproject(ICONV REQUIRED)
find_anyproject(Sqlite3 REQUIRED)
find_anyproject(ZLIB REQUIRED)
find_anyproject(LibXml2 REQUIRED)

if(BUILD_SHARED_LIBS)
    set(LIB_NAME ${PROJECT_NAME})
    add_library(${LIB_NAME} SHARED ${SOURCES})
    set_target_properties(${LIB_NAME} PROPERTIES DEFINE_SYMBOL DLL_EXPORTS)
    set_target_properties(${LIB_NAME} PROPERTIES SOVERSION 1)
else()
    set(LIB_NAME "${PROJECT_NAME}static")
    add_library(${LIB_NAME} STATIC ${SOURCES})
    set_target_properties(${LIB_NAME} PROPERTIES SOVERSION 1)
endif()

target_link_extlibraries(${LIB_NAME})

export(TARGETS ${LIB_NAME} FILE ${LIB_NAME}-exports.cmake)

#------------------------------------------------------------------------------
# targets installation
#------------------------------------------------------------------------------

install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
)

file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/headers/*.h")
install(FILES ${HEADERS}
  DESTINATION ${INSTALL_INC_DIR}
)