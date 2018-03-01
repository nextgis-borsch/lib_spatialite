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

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceRuns)
include(CheckCSourceCompiles)

if(WIN32)
    set(TARGET_CPU "Win32")
    if (MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS -DDLL_EXPORT -DYY_NO_UNISTD_H)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:precise /Ox")
        if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W3"
                CMAKE_C_FLAGS "${CMAKE_C_FLAGS}"
            )
        else()
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
        endif()

        add_definitions(-Dstrcasecmp=_stricmp -Dstrncasecmp=_strnicmp)

    endif()
else()
    exec_program(${CMAKE_C_COMPILER}
        ARGS -dumpmachine
        OUTPUT_VARIABLE TARGET_CPU)
    if(APPLE)
        add_definitions(-D_ANSI_SOURCE)
    endif()
endif()


check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("strings.h" HAVE_STRINGS_H)
check_include_file("memory.h" HAVE_MEMORY_H)
check_include_file("math.h" HAVE_MATH_H)
check_include_file("float.h" HAVE_FLOAT_H)
check_include_file("dlfcn.h" HAVE_DLFCN_H)
check_include_file("fcntl.h" HAVE_FCNTL_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("stddef.h" HAVE_STDDEF_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("unistd.h" HAVE_UNISTD_H)

set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${SQLITE3_INCLUDE_DIRS})
check_include_file("sqlite3ext.h" HAVE_SQLITE3EXT_H)

check_function_exists(getcwd HAVE_GETCWD)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(fdatasync HAVE_FDATASYNC)
check_function_exists(ftruncate HAVE_FTRUNCATE)

check_function_exists(localtime_r HAVE_LOCALTIME_R)
if(NOT WIN32)
    check_c_source_runs("
    int main ()
    {
    struct stat sbuf;
      return lstat (\"\", &sbuf) == 0;
    }" HAVE_LSTAT_EMPTY_STRING_BUG)

  check_c_source_runs("
    int main ()
    {
        struct stat sbuf;
        return stat (\"\", &sbuf) == 0;
    }" HAVE_STAT_EMPTY_STRING_BUG)

  check_c_source_runs("
    int  main ()
    {
    struct stat sbuf;
    return lstat (\"conftest.sym/\", &sbuf) == 0;
    }" LSTAT_FOLLOWS_SLASHED_SYMLINK)
endif()

check_function_exists(memmove HAVE_MEMMOVE)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(sqrt HAVE_SQRT)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(strerror HAVE_STRERROR)
check_function_exists(strftime HAVE_STRFTIME)
check_function_exists(strncasecmp HAVE_STRNCASECMP)
check_function_exists(strstr HAVE_STRSTR)

set(LT_OBJDIR .libs)

#if( CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    set(NDEBUG ON)
#endif()

set(PACKAGE ${PROJECT_NAME})
set(PACKAGE_BUGREPORT "a.furieri@lqt.it")
set(PACKAGE_NAME ${PACKAGE})
set(PACKAGE_STRING "${PACKAGE_NAME} ${VERSION}")
set(PACKAGE_TARNAME ${PACKAGE})
set(PACKAGE_URL "https://github.com/nextgis-borsch/lib_spatialite")
set(PACKAGE_VERSION ${VERSION})


check_include_file("ctype.h" HAVE_CTYPE_H)
if (HAVE_CTYPE_H AND HAVE_STDLIB_H)
    set(STDC_HEADERS 1)
endif ()

check_c_source_compiles("
    #include <sys/types.h>
    #include <time.h>
    int main ()
    {
        struct tm tm;
        int *p = &tm.tm_sec;
        return !p;
    }" TM_IN_SYS_TIME
)

check_c_source_compiles("
    #include <sys/types.h>
    #include <sys/time.h>
    #include <time.h>

    int main ()
    {
        if ((struct tm *) 0)
            return 0;
        return 0;
    }" TIME_WITH_SYS_TIME
)

set(_FILE_OFFSET_BITS 64)
set(_LARGEFILE_SOURCE ON)
set(_LARGE_FILE ON)

if(WIN32)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config-msvc.h IMMEDIATE @ONLY)
endif()

configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/config.h IMMEDIATE @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)
