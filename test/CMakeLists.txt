################################################################################
# Project:  Lib spatialite
# Purpose:  CMake build scripts
# Author:   Dmitry Baryshnikov, <dmitry.baryshnikov@nextgis.com>
################################################################################
# Copyright (C) 2018, NextGIS <info@nextgis.com>
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

if(BUILD_TESTING)

    if(LIBXML2_FOUND)
        set(WITH_LibLZMA ON CACHE INTERNAL "Get liblzma")
        if(WITH_LibXml2_EXTERNAL)
            set(WITH_LibLZMA_EXTERNAL ON CACHE INTERNAL "Get external liblzma")
        endif()
        find_anyproject(LibLZMA REQUIRED)
    endif()

    set(EXTRA_DIST
        shp
        sql_stmt_tests
    	shapetest1.dbf
    	shapetest1.prj
    	shapetest1.qpj
    	shapetest1.shp
    	shapetest1.shx
    	test-legacy-2.3.1.sqlite
    	test-legacy-3.0.1.sqlite
    	test-invalid.sqlite
    	test_geos.sqlite
    	testcase1.xls
    	testcase1.csv
    	books.xml books.xsd opera.xml opera.xsd
    	movies.xml movies.xsd books-bad.xml books-bad.xsd
    	inspire-data-example.xml stazioni_se.xml
    	stazioni2_se.xml raster_se.xml raster2_se.xml
    	sld_sample.xml sld_sample2.xml thunderstorm_mild.svg
    	tile000.jpeg  tile101.jpeg  tile111.jpeg
    	test.webp tile100.jpeg  tile110.jpeg
    	Apple-iPhone-4.jpg empty.png  empty.tif
    	test.wfs testDescribeFeatureType.wfs
    	getcapabilities-1.0.0.wfs
    	getcapabilities-1.1.0.wfs
    	describefeaturetype.wfs
    	22.dxf f06.dxf l02.dxf p05.dxf
    	archaic.dxf linked.dxf hatch.dxf
    	symbol.dxf gpkg_test.sqlite gpkg_test.gpkg
    	gpkg_test_broken.gpkg gpkg_test_extrasrid.gpkg
    )

    foreach(EXTRA ${EXTRA_DIST})
        file(COPY ${EXTRA} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()

    set(check_PROGRAMS check_endian
		check_version
		check_init
		check_init2
		check_geom_aux
		check_geometry_cols
		check_create
		check_bufovflw
		check_fdo1
		check_fdo2
		check_fdo3
		check_fdo_bufovflw
		check_md5
		check_dbf_load
		check_shp_load
		check_shp_load_3d
		shape_cp1252
		shape_primitives
		shape_utf8_1
		shape_utf8_1ex
		shape_utf8_2
		shape_3d
		check_clone_table
		check_xls_load
		check_math_funcs
		check_gaia_util
		check_gaia_utf8
		# check_extension
		check_recover_geom
		# check_sql_stmt
		# check_multithread
		check_virtualtable1
		# check_virtualtable2
		# check_virtualtable3
		check_virtualtable4
		check_virtualtable5
		check_virtualtable6
		check_virtual_ovflw
		check_mbrcache
		check_spatialindex
		check_exif
		check_exif2
		check_relations_fncts
		check_extra_relations_fncts
		check_geoscvt_fncts
		check_libxml2
		check_styling
		check_virtualxpath
		check_virtualbbox
		check_wfsin
		check_dxf
		check_metacatalog
		check_virtualelem
		check_srid_fncts
		check_control_points
    )

    if(ENABLE_GEOPACKAGE)
        set(check_PROGRAMS ${check_PROGRAMS}
    		check_createBaseTables
    		check_gpkgCreateTilesTable
    		check_gpkgCreateTilesTableMissingSRID
    		check_gpkgCreateTilesZoomLevel
    		check_gpkgInsertEpsgSRID check_gpkgMode
    		check_gpkgCreateFeaturesTable
    		check_gpkg_base_core_container_data_file_format_application_id
    		# check_gpkg_base_core_spatial_ref_sys_data_table_def
    		check_gpkg_base_core_spatial_ref_sys_data_values_default
    		# check_gpkg_base_core_contents_data_table_def
    		# check_gpkg_opt_extension_mechanism_extensions_data_table_def
    		check_add_tile_triggers
    		check_add_tile_triggers_bad_table_name
    		check_get_normal_row
    		check_get_normal_row_bad_geopackage
    		check_get_normal_row_bad_geopackage2
    		check_get_normal_zoom
    		# check_get_normal_zoom_extension_load
    		check_get_normal_zoom_bad_geopackage
    		check_get_normal_zoom_bad_geopackage2
    		check_gpkgGetImageFormat
    		check_gpkgGetImageFormat_nonimage
    		check_gpkgGetImageFormat_nonblob
    		check_gpkgGetImageFormat_png
    		check_gpkgGetImageFormat_tiff
    		check_gpkgGetImageFormat_webp
    		check_gpkgConvert
    		check_gpkgVirtual
        )
    endif()

    macro(Spatialite_TEST name)
        add_executable(${name} ${name}.c scandir4win.h)
        target_link_libraries(${name} ${LIB_NAME} ${TARGET_LINK_LIB})
        add_test(${name} ${name})
    endmacro()

    foreach(TEST ${check_PROGRAMS})
        spatialite_test(${TEST})
    endforeach()

    if(WIN32)
        file(GLOB_RECURSE IMPORTED_WIN_DLLS ${CMAKE_BINARY_DIR}/third-party/*.dll)
        foreach(IMPORTED_WIN_DLL ${IMPORTED_WIN_DLLS})
            add_custom_command(TARGET check_control_points
                                POST_BUILD
                                COMMAND ${CMAKE_COMMAND} -E copy ${IMPORTED_WIN_DLL} $<TARGET_FILE_DIR:check_control_points>
                                COMMENT "Copy ${IMPORTED_WIN_DLL} to check_control_points"
                            )
        endforeach()
    endif()

endif()
