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

set(MAIN_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/spatialite.h
)

set(SPL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gaiaexif.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gaiaaux.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gaiamatrix.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gaiageo.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_const.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_structs.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_core.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_mbr.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_formats.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_dynamic.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_advanced.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_xml.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_wfs.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/gg_dxf.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/sqlite.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/debug.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/geopackage.h
	${CMAKE_CURRENT_SOURCE_DIR}/spatialite/control_points.h
)

set(PRIV_HEADERS
    spatialite_private.h
)

set(MAIN_HEADERS ${MAIN_HEADERS} PARENT_SCOPE)

if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
    install(FILES ${MAIN_HEADERS} DESTINATION "${INSTALL_INC_DIR}")
    install(FILES ${SPL_HEADERS} DESTINATION "${INSTALL_INC_DIR}/spatialite")
endif()
