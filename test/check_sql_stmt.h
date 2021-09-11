/*

 check_sql_stmt_tiny.h -- common header file for SpatiaLite Test Cases

 Author: Sandro Furieri <a.furieri@lqt.it>

 ------------------------------------------------------------------------------
 
 Version: MPL 1.1/GPL 2.0/LGPL 2.1
 
 The contents of this file are subject to the Mozilla Public License Version
 1.1 (the "License"); you may not use this file except in compliance with
 the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/
 
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the
License.

The Original Code is the SpatiaLite library

The Initial Developer of the Original Code is Alessandro Furieri
 
Portions created by the Initial Developer are Copyright (C) 2019
the Initial Developer. All Rights Reserved.

Contributor(s):


Alternatively, the contents of this file may be used under the terms of
either the GNU General Public License Version 2 or later (the "GPL"), or
the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
in which case the provisions of the GPL or the LGPL are applicable instead
of those above. If you wish to allow use of your version of this file only
under the terms of either the GPL or the LGPL, and not to allow others to
use your version of this file under the terms of the MPL, indicate your
decision by deleting the provisions above and replace them with the notice
and other provisions required by the GPL or the LGPL. If you do not delete
the provisions above, a recipient may use your version of this file under
the terms of any one of the MPL, the GPL or the LGPL.
 
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#ifndef _WIN32
#include <dirent.h>
#include <fnmatch.h>
#endif

#include <spatialite/gaiaconfig.h>

#include "sqlite3.h"
#include "spatialite.h"

#ifndef OMIT_GEOS		/* including GEOS */
#ifdef GEOS_REENTRANT
#ifdef GEOS_ONLY_REENTRANT
#define GEOS_USE_ONLY_R_API	/* only fully thread-saextern void
				   close_connection (struct db_conn *conn)fe GEOS API */
#endif
#endif
#include <geos_c.h>
#endif

#ifndef OMIT_PROJ		/* only if PROJ is supported */
#ifdef PROJ_NEW			/* supporting new PROJ.6 */
#include <proj.h>
#else /* supporting old PROJ.6 */
#include <proj_api.h>
#endif
#endif

#ifdef _WIN32
#include "fnmatch4win.h"
#include "scandir4win.h"
#include "fnmatch_impl4win.h"
#endif

struct test_data
{
    char *test_case_name;
    char *database_name;
    char *sql_statement;
    int expected_rows;
    int expected_columns;
    char **expected_results;
    int *expected_precision;
};

struct db_conn
{
    sqlite3 *db_handle;
    char *db_path;
    int read_only;
    const void *cache;
};


extern int
run_all_testcases (struct db_conn *conn, int load_extension, int legacy);
extern int
run_specified_testcases (int argc, char *argv[], struct db_conn *conn,
			 int load_extension);
extern void close_connection (struct db_conn *conn);


extern void
close_connection (struct db_conn *conn)
{
    if (conn->db_path != NULL)
	free (conn->db_path);
    conn->db_path = NULL;
    if (conn->db_handle != NULL)
	sqlite3_close (conn->db_handle);
    conn->db_handle = NULL;
    if (conn->cache == NULL)
	spatialite_cleanup ();
}

static void
save_connection (struct db_conn *conn, const char *database_name,
		 sqlite3 * db_handle, int read_only, int empty_db)
{
    int len = strlen (database_name);
    conn->db_handle = db_handle;
    conn->read_only = read_only;
    if (read_only)
      {
	  conn->db_path = malloc (len - 2);
	  memcpy (conn->db_path, database_name, len - 3);
	  *(conn->db_path + len - 3) = '\0';
      }
    else if (empty_db)
      {
	  conn->db_path = malloc (9);
	  strcpy (conn->db_path, ":memory:");
      }
    else
      {
	  conn->db_path = malloc (len + 1);
	  strcpy (conn->db_path, database_name);
      }
}

static int
compare_path (const char *pth1, const char *pth2, int read_only)
{
    int ret = 0;
    if (!read_only)
      {
	  if (strcmp (pth1, pth2) == 0)
	      ret = 1;
      }
    else
      {
	  if (strncmp (pth1, pth2, strlen (pth2) - 3) == 0)
	      ret = 1;
      }
    return ret;
}

static int
load_dyn_extension (sqlite3 * db_handle)
{
    int ret;
    char *err_msg = NULL;
    sqlite3_enable_load_extension (db_handle, 1);
    ret =
	sqlite3_exec (db_handle, "SELECT load_extension('mod_spatialite')",
		      NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "load_extension() error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return 0;
      }
    return 1;
}

static int
do_one_case (struct db_conn *conn, const struct test_data *data,
	     int load_extension, int gpkg_amphibious_mode)
{
    sqlite3 *db_handle = NULL;
    int ret;
    char *err_msg = NULL;
    int i;
    char **results;
    int rows;
    int columns;
    int read_only = 0;
    int empty_db = 0;
    int not_memory_db = 0;

    fprintf (stderr, "Test case: %s\n", data->test_case_name);
    if (strncmp
	("_RO", data->database_name + strlen (data->database_name) - 3,
	 strlen ("_RO")) == 0)
	read_only = 1;
    if (strcmp ("NEW:memory:", data->database_name) == 0)
	empty_db = 1;
    if (conn->db_handle != NULL)
      {
	  if (empty_db)
	      ;
	  else if (compare_path (conn->db_path, data->database_name, read_only))
	    {
		if (conn->read_only == read_only)
		  {
		      db_handle = conn->db_handle;
		      goto skip_init;
		  }
	    }
	  close_connection (conn);
      }

    if (conn->cache == NULL && !load_extension)
	spatialite_init (0);

    /* This hack checks if the name ends with _RO */
    if (strncmp
	("_RO", data->database_name + strlen (data->database_name) - 3,
	 strlen ("_RO")) == 0)
      {
	  fprintf (stderr, "opening read_only\n");
	  read_only = 1;
	  ret =
	      sqlite3_open_v2 (data->database_name, &db_handle,
			       SQLITE_OPEN_READONLY, NULL);
      }
    else if (empty_db)
      {
	  ret =
	      sqlite3_open_v2 (":memory:", &db_handle,
			       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
			       NULL);
      }
    else
      {
	  if (strcmp (data->database_name, ":memory:") != 0)
	      not_memory_db = 1;
	  ret =
	      sqlite3_open_v2 (data->database_name, &db_handle,
			       SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
			       NULL);
      }
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "cannot open %s db: %s\n", data->database_name,
		   sqlite3_errmsg (db_handle));
	  sqlite3_close (db_handle);
	  db_handle = NULL;
	  return -1;
      }

    if (conn->cache != NULL)
	spatialite_init_ex (db_handle, conn->cache, 0);
    if (load_extension)
      {
	  if (!load_dyn_extension (db_handle))
	    {
		sqlite3_close (db_handle);
		db_handle = NULL;
		return -3;
	    }
      }
    save_connection (conn, data->database_name, db_handle, read_only, empty_db);

    if (read_only || not_memory_db)
	goto skip_init;
    ret =
	sqlite3_exec (db_handle, "SELECT InitSpatialMetadataFull(1)", NULL,
		      NULL, &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "InitSpatialMetadataFull() error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -2;
      }
    ret =
	sqlite3_exec (db_handle, "SELECT StoredProc_CreateTables()", NULL, NULL,
		      &err_msg);
    if (ret != SQLITE_OK)
      {
	  fprintf (stderr, "StoredProc_CreateTables() error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -2;
      }
  skip_init:

    if (gpkg_amphibious_mode)
      {
	  sqlite3_exec (db_handle, "SELECT EnableGpkgAmphibiousMode()",
			NULL, NULL, NULL);
      }
    ret =
	sqlite3_get_table (db_handle, data->sql_statement, &results, &rows,
			   &columns, &err_msg);
    if (gpkg_amphibious_mode)
      {
	  sqlite3_exec (db_handle, "SELECT DisableGpkgAmphibiousMode()",
			NULL, NULL, NULL);
      }
    if (ret != SQLITE_OK)
      {
	  if (data->expected_rows == 1 && data->expected_columns == 1)
	    {
		/* checking for an expected exception */
		if (strcmp (err_msg, data->expected_results[1]) == 0)
		  {
		      /* we expected this */
		      sqlite3_free (err_msg);
		      sqlite3_free_table (results);
		      return 0;
		  }
	    }
	  fprintf (stderr, "Error: %s\n", err_msg);
	  sqlite3_free (err_msg);
	  return -10;
      }
    if ((rows != data->expected_rows) || (columns != data->expected_columns))
      {
	  fprintf (stderr, "Unexpected error: bad result: %i/%i.\n", rows,
		   columns);
	  sqlite3_free_table (results);
	  return -11;
      }
    for (i = 0; i < (data->expected_rows + 1) * data->expected_columns; ++i)
      {
	  if (results[i] != NULL && data->expected_precision[i] == 0)
	    {
		data->expected_precision[i] = strlen (results[i]);
	    }
	  if (results[i] == NULL)
	    {
		if (strcmp ("(NULL)", data->expected_results[i]) == 0)
		  {
		      /* we expected this */
		      continue;
		  }
		else
		  {
		      fprintf (stderr, "Null value at %i.\n", i);
		      fprintf (stderr, "Expected value was: %s\n",
			       data->expected_results[i]);
		      sqlite3_free_table (results);
		      return -12;
		  }
	    }
	  else if (strlen (results[i]) == 0)
	    {
		fprintf (stderr, "zero length result at %i\n", i);
		fprintf (stderr, "Expected value was    : %s|\n",
			 data->expected_results[i]);
		sqlite3_free_table (results);
		return -13;
	    }
	  else if (strncmp
		   (results[i], data->expected_results[i],
		    data->expected_precision[i]) != 0)
	    {
		fprintf (stderr, "Unexpected value at %i: %s|\n", i,
			 results[i]);
		fprintf (stderr, "Expected value was   : %s|\n",
			 data->expected_results[i]);
		sqlite3_free_table (results);
		return -14;
	    }
      }
    sqlite3_free_table (results);

    return 0;
}

static int
get_clean_line (FILE * f, char **line)
{
    size_t len = 0;
    ssize_t num_read = 0;
    ssize_t end = 0;
    char *tmp_line = NULL;

#if !defined(_WIN32) &&!defined(__APPLE__)
/* expecting to be on a sane minded platform [linux-like] */
    num_read = getline (&(tmp_line), &len, f);
#else
/* neither Windows nor MacOsX support getline() */
    len = 1024 * 1024;
    tmp_line = malloc (len);
    if (fgets (tmp_line, len, f) == NULL)
      {
	  free (tmp_line);
	  num_read = -1;
      }
    else
	num_read = strlen (tmp_line);
#endif

    if (num_read < 1)
      {
	  fprintf (stderr, "failed to read at %li: %li\n", ftell (f), num_read);
	  return -1;
      }
    /* trim the trailing new line and any comments */
    for (end = 0; end < num_read; ++end)
      {
	  if (*(tmp_line + end) == '\n')
	      break;
	  if (*(tmp_line + end) == '#')
	      break;
      }
    /* trim any trailing spaces */
    while (end > 0)
      {
	  if (*(tmp_line + end - 1) != ' ')
	    {
		break;
	    }
	  *(tmp_line + end - 1) = '\0';
	  end--;
      }
    *line = malloc (end + 1);
    memcpy (*line, tmp_line, end);
    (*line)[end] = '\0';
    free (tmp_line);
    return 0;
}

static void
handle_precision (char *expected_result, int *precision)
{
    int i;
    int prcsn;
    int result_len = strlen (expected_result);
    *precision = 0;
    for (i = result_len - 1; i >= 0; --i)
      {
	  if (expected_result[i] == ':')
	    {
		prcsn = atoi (&(expected_result[i + 1]));
		if (prcsn > 0)
		  {
		      expected_result[i] = '\0';
		      *precision = prcsn;
		  }
		break;
	    }
      }
}

static struct test_data *
read_one_case (const char *filepath)
{
    int num_results;
    int i;
    char *tmp;
    FILE *f;
    struct test_data *data;

    f = fopen (filepath, "r");

    data = malloc (sizeof (struct test_data));
    get_clean_line (f, &(data->test_case_name));
    get_clean_line (f, &(data->database_name));
    get_clean_line (f, &(data->sql_statement));
    get_clean_line (f, &(tmp));
    data->expected_rows = atoi (tmp);
    free (tmp);
    get_clean_line (f, &(tmp));
    data->expected_columns = atoi (tmp);
    free (tmp);
    num_results = (data->expected_rows + 1) * data->expected_columns;
    data->expected_results = malloc (num_results * sizeof (char *));
    data->expected_precision = malloc (num_results * sizeof (int));
    for (i = 0; i < num_results; ++i)
      {
	  get_clean_line (f, &(data->expected_results[i]));
	  handle_precision (data->expected_results[i],
			    &(data->expected_precision[i]));
      }
    fclose (f);
    return data;
}

static void
cleanup_test_data (struct test_data *data)
{
    int i;
    int num_results = (data->expected_rows + 1) * (data->expected_columns);

    for (i = 0; i < num_results; ++i)
      {
	  free (data->expected_results[i]);
      }
    free (data->expected_results);
    free (data->expected_precision);
    free (data->test_case_name);
    free (data->database_name);
    free (data->sql_statement);
    free (data);
}

static int
test_case_filter (const struct dirent *entry)
{
    return (fnmatch ("*.testcase", entry->d_name, FNM_PERIOD) == 0);
}

static int
run_subdir_test (const char *subdirname, struct db_conn *conn,
		 int load_extension, int gpkg_amphibious_mode)
{
    struct dirent **namelist;
    int n;
    int i;
    int result = 0;

    n = scandir (subdirname, &namelist, test_case_filter, alphasort);
    if (n < 0)
      {
	  perror ("scandir");
	  return -1;
      }

    for (i = 0; i < n; ++i)
      {
	  struct test_data *data;
	  char *path =
	      sqlite3_mprintf ("%s/%s", subdirname, namelist[i]->d_name);
	  data = read_one_case (path);
	  sqlite3_free (path);

	  result =
	      do_one_case (conn, data, load_extension, gpkg_amphibious_mode);

	  cleanup_test_data (data);
	  if (result != 0)
	      break;
      }
/* cleaning up */
    for (i = 0; i < n; ++i)
	free (namelist[i]);
    free (namelist);

    return result;
}

extern int
run_all_testcases (struct db_conn *conn, int load_extension, int legacy)
{
    int ret;
    int result = 0;
    const char *security_level;
    int tiny_point = is_tiny_point_enabled (conn->cache);

    result = run_subdir_test ("sql_stmt_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }

    if (tiny_point)
      {
	  result =
	      run_subdir_test ("sql_stmt_tiny_point", conn, load_extension, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }
    else
      {
	  result =
	      run_subdir_test ("sql_stmt_point_geom", conn, load_extension, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }

    security_level = getenv ("SPATIALITE_SECURITY");
    if (security_level == NULL)
	;
    else if (strcasecmp (security_level, "relaxed") == 0)
      {
	  result =
	      run_subdir_test ("sql_stmt_security_tests", conn, load_extension,
			       0);
	  if (result != 0)
	    {
		return result;
	    }
	  if (!legacy)
	    {
		result =
		    run_subdir_test ("sql_stmt_logfile_tests", conn,
				     load_extension, 0);
		if (result != 0)
		  {
		      return result;
		  }
#ifdef ENABLE_MINIZIP		/* only if MINIZIP is enabled */
		result =
		    run_subdir_test ("sql_stmt_minizip", conn,
				     load_extension, 0);
		if (result != 0)
		  {
		      return result;
		  }
#endif

#ifdef PROJ_NEW			/* only if PROJ.6 or later is supported */
		result =
		    run_subdir_test ("sql_stmt_proj600security_tests", conn,
				     load_extension, 0);
		if (result != 0)
		  {
		      return result;
		  }
#ifdef ENABLE_MINIZIP		/* only if MINIZIP is enabled */
		result =
		    run_subdir_test ("sql_stmt_zip_proj6", conn,
				     load_extension, 0);
		if (result != 0)
		  {
		      return result;
		  }
#endif /* end MINIZIP */
#endif /* end PROJ.6 */
	    }
      }

    if (legacy)
      {
	  /* skipping Sequence tests in legacy mode */
	  fprintf (stderr,
		   "WARNING: skipping Sequence testcases in legacy mode !!!\n");
	  goto skip_sequence;
      }

    result =
	run_subdir_test ("sql_stmt_sequence_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
  skip_sequence:

    if (legacy)
      {
	  /* skipping Routing tests in legacy mode */
	  fprintf (stderr,
		   "WARNING: skipping CreateRouting testcases in legacy mode !!!\n");
	  goto skip_routing;
      }

    result =
	run_subdir_test ("sql_stmt_routing_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
  skip_routing:

    if (legacy)
      {
	  /* skipping PostgresL tests in legacy mode */
	  fprintf (stderr,
		   "WARNING: skipping Postgres testcases in legacy mode !!!\n");
	  goto skip_postgres;
      }

    result =
	run_subdir_test ("sql_stmt_postgres_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
  skip_postgres:

#ifndef OMIT_MATHSQL		/* only if MATHSQL is supported */
    result =
	run_subdir_test ("sql_stmt_mathsql_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
#endif /* end MATHSQL conditional */

#ifndef OMIT_EPSG		/* only if full EPSG is supported */
#ifndef OMIT_PROJ		/* only if PROJ is supported */
    result = run_subdir_test ("sql_stmt_proj_tests", conn, load_extension, 0);
#ifdef PROJ_NEW			/* supporting new PROJ.6 */
    if (!legacy)
      {
	  int is_720_or_later = 0;
	  if (PROJ_VERSION_MAJOR > 7)
	      is_720_or_later = 1;
	  else if (PROJ_VERSION_MAJOR == 7 && PROJ_VERSION_MINOR >= 2)
	      is_720_or_later = 1;
	  if (is_720_or_later)
	      result =
		  run_subdir_test ("sql_stmt_proj720_tests", conn,
				   load_extension, 0);
	  else
	      result =
		  run_subdir_test ("sql_stmt_proj600_tests", conn,
				   load_extension, 0);
      }
    else
	result = 0;
#else /* supporting old PROJ.4 */
    if (PJ_VERSION >= 493)
	result =
	    run_subdir_test ("sql_stmt_proj493_tests", conn, load_extension, 0);
    else
	result =
	    run_subdir_test ("sql_stmt_proj492_tests", conn, load_extension, 0);
#endif
    if (result != 0)
      {
	  return result;
      }
#endif /* end PROJ conditional */
#endif /* end EPSG conditional */

#ifndef OMIT_GEOS		/* only if GEOS is supported */
    if (strcmp (GEOSversion (), "3.3") < 0)
      {
	  /* 
	     skipping GEOS tests if some obsolete version is found 

	     rationale: obsolete versions may return substantially
	     different results, thus causing many testcases to fail
	   */
	  fprintf (stderr,
		   "WARNING: skipping GEOS testcases; obsolete version found !!!\n");
	  goto skip_geos;
      }
#ifdef GEOS_USE_ONLY_R_API	/* only fully thread-safe GEOS API */
    if (legacy)
      {
	  /* skipping GEOS tests in legacy mode */
	  fprintf (stderr,
		   "WARNING: skipping GEOS testcases in legacy mode:  GEOS_USE_ONLY_R_API defined !!!\n");
	  goto skip_geos;
      }
#endif
    result = run_subdir_test ("sql_stmt_geos_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
    if (strcmp (GEOSversion (), "3.8") >= 0)
      {
	  /* GEOS 3.8.0 changed historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_geos_380", conn, load_extension, 0);
      }
    else
      {
	  /* older versions supporting historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_geos_non380", conn, load_extension, 0);
      }
    if (result != 0)
	return result;
    if (strcmp (GEOSversion (), "3.9") >= 0)
      {
	  /* GEOS 3.9.0 changed historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_geos_390", conn, load_extension, 0);
      }
    else
      {
	  /* older versions supporting historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_geos_non390", conn, load_extension, 0);
      }
    if (result != 0)
	return result;

  skip_geos:
#endif /* end GEOS conditional */

#ifdef GEOS_ADVANCED		/* only if GEOS_ADVANCED is supported */
    if (strcmp (GEOSversion (), "3.4") < 0)
      {
	  /* 
	     skipping GEOS tests if some obsolete version is found 

	     rationale: obsolete versions may return substantially
	     different results, thus causing many testcases to fail
	   */
	  fprintf (stderr,
		   "WARNING: skipping GEOS_ADVANCED testcases; obsolete version found !!!\n");
	  goto skip_geos_advanced;
      }
#ifdef GEOS_USE_ONLY_R_API	/* only fully thread-safe GEOS API */
    if (legacy)
      {
	  /* skipping GEOS_ADVANCED tests in legacy mode */
	  fprintf (stderr,
		   "WARNING: skipping GEOS_ADVANCED testcases in legacy mode:  GEOS_USE_ONLY_R_API defined !!!\n");
	  goto skip_geos_advanced;
      }
#endif

    result =
	run_subdir_test ("sql_stmt_geosadvanced_tests", conn, load_extension,
			 0);
    if (result != 0)
      {
	  return result;
      }

#ifdef GEOS_REENTRANT
    result =
	run_subdir_test ("sql_stmt_voronoj2_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
    if (strcmp (GEOSversion (), "3.9") >= 0)
      {
	  /* GEOS 3.9.0 changed historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_voronoj_390", conn, load_extension, 0);
      }
    else
      {
	  /* older versions supporting historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_voronoj_non390", conn, load_extension, 0);
      }
#else
    result =
	run_subdir_test ("sql_stmt_voronoj1_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
#endif

  skip_geos_advanced:
#endif /* end GEOS_ADVANCED conditional */

#ifdef GEOS_370			/* only if GEOS_370 is supported */

    result =
	run_subdir_test ("sql_stmt_geos370_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }

#endif /* end GEOS_370 conditional */

#ifdef ENABLE_RTTOPO		/* only if RTTOPO is supported */
    if (legacy)
      {
	  /* skipping RTTOPO tests in legacy mode */
	  fprintf (stderr,
		   "WARNING: skipping RTTOPO testcases in legacy mode !!!\n");
	  goto skip_rttopo;
      }

    result = run_subdir_test ("sql_stmt_rtgeom_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
    if (strcmp (GEOSversion (), "3.9") >= 0)
      {
	  /* GEOS 3.9.0 changed historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_rtgeom_390", conn, load_extension, 0);
      }
    else
      {
	  /* older versions supporting historical beheviour for some tests */
	  result =
	      run_subdir_test ("sql_stmt_rtgeom_non390", conn, load_extension, 0);
      }

    result = run_subdir_test ("sql_stmt_rttopo_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }

  skip_rttopo:

#endif /* end RTTOPO conditional */

#ifndef OMIT_ICONV		/* only if ICONV is supported */
    result = run_subdir_test ("sql_stmt_iconv_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }
    if (legacy)
      {
	  /* skipping SqlProc tests in legacy mode */
	  fprintf (stderr,
		   "WARNING: skipping SqlProc testcases in legacy mode !!!\n");
	  goto skip_sql_proc;
      }

    ret =
	system
	("cp sql_stmt_proc_tests/storproc.sqlite sql_stmt_proc_tests/storproc_x.sqlite");
    if (ret != 0)
      {
	  fprintf (stderr,
		   "cannot copy sql_stmt_proc_tests/storproc database\n");
	  return -1;
      }

    result = run_subdir_test ("sql_stmt_proc_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }

  skip_sql_proc:
#endif /* end ICONV */

#ifdef ENABLE_LIBXML2		/* only if LIBXML2 is supported */
#ifndef OMIT_ICONV		/* only if ICONV is supported */
    result =
	run_subdir_test ("sql_stmt_libxml2_tests", conn, load_extension, 0);
    if (result != 0)
      {
	  return result;
      }

    security_level = getenv ("SPATIALITE_SECURITY");
    if (security_level == NULL)
	;
    else if (strcasecmp (security_level, "relaxed") == 0)
      {
	  result =
	      run_subdir_test ("sql_stmt_xmlsec_tests", conn, load_extension,
			       0);
	  if (result != 0)
	    {
		return result;
	    }
      }

#endif
#endif /* end LIBXML2 conditional */

#ifdef ENABLE_LIBXML2		/* only if LIBXML2 is supported */
#ifdef ENABLE_RTTOPO		/* only if RTTOPO is supported */
    result =
	run_subdir_test ("sql_stmt_libxml2_rttopo_tests", conn, load_extension,
			 0);
    if (result != 0)
      {
	  return result;
      }
#endif
#endif /* end LIBXML2 conditional */

#ifdef ENABLE_GEOPACKAGE	/* only if GeoPackage support is enabled */
    if (!legacy)
      {
	  result =
	      run_subdir_test ("sql_stmt_geopackage_tests", conn,
			       load_extension, 1);
	  if (result != 0)
	    {
		return result;
	    }
#ifndef OMIT_EPSG		/* EPSG is supported */
#ifdef PROJ_NEW			/* supporting new PROJ.6 */
	  result =
	      run_subdir_test ("sql_stmt_gpkg_epsg600_tests", conn,
			       load_extension, 1);
#else /* supporting old PROJ.4 */
	  if (PJ_VERSION >= 493)
	      result =
		  run_subdir_test ("sql_stmt_gpkg_epsg493_tests", conn,
				   load_extension, 1);
	  else
	      result =
		  run_subdir_test ("sql_stmt_gpkg_epsg492_tests", conn,
				   load_extension, 1);
#endif
	  if (result != 0)
	    {
		return result;
	    }
#endif /* end EPSG conditional */
      }

#endif /* end GEOPACKAGE conditional */

#ifndef OMIT_FREEXL		/* FREEXL is enabled */
    security_level = getenv ("SPATIALITE_SECURITY");
    if (security_level == NULL)
	;
    else if (strcasecmp (security_level, "relaxed") == 0)
      {
	  result =
	      run_subdir_test ("sql_stmt_freexl_tests", conn, load_extension,
			       0);
	  if (result != 0)
	    {
		return result;
	    }
      }
#endif /* end FREEXL support */

/* testing global settings SQL functions */
    if (legacy)
      {
	  result =
	      run_subdir_test ("sql_stmt_nocache_tests", conn, load_extension,
			       0);
	  if (result != 0)
	    {
		return result;
	    }
      }
    else
      {
	  result =
	      run_subdir_test ("sql_stmt_cache_tests", conn, load_extension, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }

#ifdef ENABLE_GEOPACKAGE	/* only if GeoPackage support is enabled */
    if (legacy)
      {
	  result =
	      run_subdir_test ("sql_stmt_gpkgnocache_tests", conn,
			       load_extension, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }
    else
      {
	  result =
	      run_subdir_test ("sql_stmt_gpkgcache_tests", conn, load_extension,
			       0);
	  if (result != 0)
	    {
		return result;
	    }
      }
#endif /* end GEOPACKAGE conditional */

/* Rename Table/Column */
    if (sqlite3_libversion_number () < 3025000)
      {
	  result = run_subdir_test ("sql_stmt_renameold_tests", conn, 0, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }
    else
      {
	  result = run_subdir_test ("sql_stmt_renamenew_tests", conn, 0, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }

/* BufferOptions */
    if (legacy)
      {
	  result = run_subdir_test ("sql_stmt_bufoptsold_tests", conn, 0, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }
    else
      {
	  result = run_subdir_test ("sql_stmt_bufoptsnew_tests", conn, 0, 0);
	  if (result != 0)
	    {
		return result;
	    }
      }

    return result;
}

extern int
run_specified_testcases (int argc, char *argv[], struct db_conn *conn,
			 int load_extension)
{
    int result = 0;
    int i = 0;

    for (i = 1; i < argc; ++i)
      {
	  struct test_data *data;
	  data = read_one_case (argv[i]);
	  result = do_one_case (conn, data, load_extension, 0);
	  cleanup_test_data (data);
	  if (result != 0)
	    {
		break;
	    }
      }
    return result;
}
