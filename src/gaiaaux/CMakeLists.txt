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

set(TARGET_NAME gaiaaux)

set(CSOURCES gg_sqlaux.c)

if(MSVC_VERSION GREATER 1600)
    # This is HACK for strange and not normal dependency of library on OSGeo4W

    file(READ gg_utf8.c gg_utf8_CONTENTS)
    string(REPLACE "#if defined(__MINGW32__) || defined(_WIN32)" "#if 0" gg_utf8_CONTENTS "${gg_utf8_CONTENTS}")
    string(REPLACE "#if defined(__APPLE__) || defined(__ANDROID__)" "#if 1" gg_utf8_CONTENTS "${gg_utf8_CONTENTS}")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gg_utf8.c "${gg_utf8_CONTENTS}")

    set(CSOURCES ${CSOURCES}
    	${CMAKE_CURRENT_BINARY_DIR}/gg_utf8.c
    )

else()
    set(CSOURCES ${CSOURCES}
    	gg_utf8.c
    )
endif()

add_library(${TARGET_NAME} OBJECT ${CSOURCES})

# build target
set(TARGET_OBJECTS ${TARGET_OBJECTS} $<TARGET_OBJECTS:${TARGET_NAME}> PARENT_SCOPE)
