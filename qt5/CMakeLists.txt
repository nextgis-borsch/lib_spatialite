################################################################################
# Project:  spatialite
# Purpose:  CMake build scripts
# Author:   Dmitry Baryshnikov, dmitry.baryshnikov@nextgis.com
################################################################################
# Copyright (C) 2020, NextGIS <info@nextgis.com>
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

find_anyproject(Qt5 REQUIRED COMPONENTS Core Sql)
set(CMAKE_AUTOMOC ON)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

include_directories(SYSTEM
    ${SQLITE3_INCLUDE_DIR}
    ${Qt5Sql_PRIVATE_INCLUDE_DIRS}
)

set(QSQLSPATIALITE_SRC 
    qsql_spatialite.cpp
    smain.cpp
    qgssqliteutils.cpp
    qgsspatialiteutils.cpp
)

add_library(qsqlspatialite SHARED ${QSQLSPATIALITE_SRC})

target_link_libraries(qsqlspatialite
    Qt5::Core
    Qt5::Sql
    ${LIB_NAME}
    ${SPATIALITE_LIBRARY}
    ${SQLITE3_LIBRARIES}
)

install(TARGETS qsqlspatialite
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}/qt5/plugins/sqldrivers
  RUNTIME DESTINATION ${INSTALL_LIB_DIR}/qt5/plugins/sqldrivers
)

if(OSX_FRAMEWORK)
    set(INSTALL_MODULES_DIR Library/Plugins/Qt5/sqldrivers)
else()
    set(INSTALL_MODULES_DIR ${INSTALL_LIB_DIR}/qt5/plugins/sqldrivers)
endif()

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
    install(TARGETS qsqlspatialite
        RUNTIME DESTINATION ${INSTALL_MODULES_DIR}
        LIBRARY DESTINATION ${INSTALL_MODULES_DIR}
        ARCHIVE DESTINATION ${INSTALL_MODULES_DIR}
        INCLUDES DESTINATION ${INSTALL_INC_DIR}
        FRAMEWORK DESTINATION ${INSTALL_MODULES_DIR}
    )
endif()
