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

set(TARGET_NAME gaiageo)

set(CSOURCES gg_advanced.c
	gg_endian.c
	gg_geodesic.c
	gg_geometries.c
	gg_geoscvt.c
	gg_relations.c
	gg_relations_ext.c
	gg_lwgeom.c
	gg_shape.c
	gg_transform.c
	gg_wkb.c
	gg_wkt.c
	gg_vanuatu.c
	gg_ewkt.c
	gg_geoJSON.c
	gg_kml.c
	gg_gml.c
	gg_voronoj.c
	gg_xml.c
	gg_matrix.c
#    Ewkt.c lex.Ewkt.c
#	geoJSON.c lex.GeoJson.c
#	Gml.h Gml.c lex.Gml.c
#	Kml.h Kml.c lex.Kml.c
#	vanuatuWkt.c lex.VanuatuWkt.c
)

if(MSVC_VERSION GREATER_OR_EQUAL_TO 1700)
    # The rint is already defied so replace it rint_tmp
    file(READ gg_extras.c gg_extras_CONTENTS)
    string(REPLACE "rint (double x)" "rint_tmp (double x)" gg_extras_CONTENTS "${gg_extras_CONTENTS}")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gg_extras.c "${gg_extras_CONTENTS}")
    set(CSOURCES ${CSOURCES}
    	${CMAKE_CURRENT_BINARY_DIR}/gg_extras.c
    )
else()
    set(CSOURCES ${CSOURCES}
    	gg_extras.c
    )
endif()

add_library(${TARGET_NAME} OBJECT ${CSOURCES})

# build target
set(TARGET_OBJECTS ${TARGET_OBJECTS} $<TARGET_OBJECTS:${TARGET_NAME}> PARENT_SCOPE)
