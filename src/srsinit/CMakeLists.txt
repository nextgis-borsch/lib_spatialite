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

if(NOT OMIT_EPSG)

set(TARGET_NAME srsinit)

set(CSOURCES srs_init.c
	epsg_inlined_00.c
	epsg_inlined_01.c
	epsg_inlined_02.c
	epsg_inlined_03.c
	epsg_inlined_04.c
	epsg_inlined_05.c
	epsg_inlined_06.c
	epsg_inlined_07.c
	epsg_inlined_08.c
	epsg_inlined_09.c
	epsg_inlined_10.c
	epsg_inlined_11.c
	epsg_inlined_12.c
	epsg_inlined_13.c
	epsg_inlined_14.c
	epsg_inlined_15.c
	epsg_inlined_16.c
	epsg_inlined_17.c
	epsg_inlined_18.c
	epsg_inlined_19.c
	epsg_inlined_20.c
	epsg_inlined_21.c
	epsg_inlined_22.c
	epsg_inlined_23.c
	epsg_inlined_24.c
	epsg_inlined_25.c
	epsg_inlined_26.c
	epsg_inlined_27.c
	epsg_inlined_28.c
	epsg_inlined_29.c
	epsg_inlined_30.c
	epsg_inlined_31.c
	epsg_inlined_32.c
	epsg_inlined_33.c
	epsg_inlined_34.c
	epsg_inlined_35.c
	epsg_inlined_36.c
	epsg_inlined_37.c
	epsg_inlined_38.c
	epsg_inlined_39.c
	epsg_inlined_40.c
	epsg_inlined_41.c
	epsg_inlined_42.c
	epsg_inlined_43.c
	epsg_inlined_44.c
	epsg_inlined_45.c
	epsg_inlined_46.c
	epsg_inlined_47.c
	epsg_inlined_48.c
	epsg_inlined_49.c
	epsg_inlined_50.c
	epsg_inlined_51.c
	epsg_inlined_52.c
	epsg_inlined_53.c
	epsg_inlined_54.c
	epsg_inlined_55.c
	epsg_inlined_56.c
	epsg_inlined_57.c
	epsg_inlined_58.c
	epsg_inlined_59.c
	epsg_inlined_wgs84_00.c
	epsg_inlined_wgs84_01.c
	epsg_inlined_prussian.c
	epsg_inlined_extra.c
)

add_library(${TARGET_NAME} OBJECT ${CSOURCES})

# build target
set(TARGET_OBJECTS ${TARGET_OBJECTS} $<TARGET_OBJECTS:${TARGET_NAME}> PARENT_SCOPE)
endif()
