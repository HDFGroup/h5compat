/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have          *
 * access to either file, you may request a copy from help@hdfgroup.org.     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include "hdf5.h"

/*
 * Basic tests of error (H5E) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

typedef struct err_info_t {
    const char *file;           /* "file name" for error */
    const char *func;           /* "function name" for error */
    unsigned line;              /* "line number" for error */
    H5E_major_t maj;            /* "major error value" for error */
    H5E_minor_t min;            /* "minor error value" for error */
    const char *str;            /* "information string" for error */
} err_info_t;

/* Array to hold pre-initialized errors */
err_info_t errors[5];

static void
init_array(void)
{
    errors[0].file = "file1";
    errors[0].func = "func1";
    errors[0].line = 1;
    errors[0].maj = H5E_ATOM;
    errors[0].min = H5E_UNINITIALIZED;
    errors[0].str = "str1";
    errors[1].file = "file2";
    errors[1].func = "func2";
    errors[1].line = 2;
    errors[1].maj = H5E_CACHE;
    errors[1].min = H5E_UNSUPPORTED;
    errors[1].str = "str2";
    errors[2].file = "file3";
    errors[2].func = "func3";
    errors[2].line = 3;
    errors[2].maj = H5E_BTREE;
    errors[2].min = H5E_BADTYPE;
    errors[2].str = "str3";
    errors[3].file = "file4";
    errors[3].func = "func4";
    errors[3].line = 4;
    errors[3].maj = H5E_SYM;
    errors[3].min = H5E_BADRANGE;
    errors[3].str = "str4";
    errors[4].file = "file5";
    errors[4].func = "func5";
    errors[4].line = 5;
    errors[4].maj = H5E_HEAP;
    errors[4].min = H5E_BADVALUE;
    errors[4].str = "str5";
}

static herr_t
auto_error(void *client_data)
{
    /* Shut compiler up */
    client_data = client_data;

    return(0);
}

static herr_t
walk_error(int n, H5E_error_t *err_desc, void *client_data)
{
    /* Shut compiler up */
    client_data = client_data;

    /* Check for correct error on stack */
    if((unsigned)n != (err_desc->line - 1))
        return(-1);
    else
        return(0);
}


int
main(int argc, const char *argv[])
{
    H5E_auto_t efunc;   /* Error stack func */
    void *edata;        /* Error stack op_data */
    size_t u;           /* Local index variable */

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Initialize error array */
    init_array();

    /* Set up for pushing our error on stack */
    if(H5Eclear() < 0) goto error;
    if(H5Eget_auto(&efunc, &edata) < 0) goto error;
    if(H5Eset_auto(auto_error, NULL) < 0) goto error;

    /* Push errors on stack */
    for(u = 0; u < (sizeof(errors) / sizeof(errors[0])); u++)
        if(H5Epush(errors[u].file, errors[u].func, errors[u].line, errors[u].maj,
                errors[u].min, errors[u].str) < 0) goto error;

    /* Walk the error stack */
    if(H5Ewalk(H5E_WALK_UPWARD, walk_error, NULL) < 0) goto error;

    /* Print the error stack */
    if(H5Eprint(stdout) < 0) goto error;

    /* Restore previous error reporting reoutine */
    if(H5Eset_auto(efunc, edata) < 0) goto error;

    return(0);

error:
    return(1);
}

