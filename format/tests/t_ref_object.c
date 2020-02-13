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

/*
 *  This can be compiled on all library release versions with:
 *      h5cc tests/t_ref_object.c
 *
 *  This will add a referenced dataset to the existing test file FILENAME:
 *  --for library release 1.12 and above, it will create with the revised reference type
 *      "Add_revised_ref_object"
 *  --for library release 1.10 and below, it will create with the old reference type
 *      "Add_old_ref_object"
 */

#include "hdf5.h"
#include <stdlib.h>

/* The test file */
#define FILENAME "ref_compat.h5"    

/* Determine the maximum value */
#define MAX(a,b) (((a)>(b)) ? (a) : (b))

int main(int argc, char *argv[])
{
    hid_t fid = -1;		/* File ID */
    hid_t gid = -1;     /* Group ID */
    hid_t did = -1;     /* Dataset ID */
    hid_t sid = -1;	    /* Dataspace ID */
    hsize_t dims1[] = {2}; /* Dimension size */
    int i;              /* Local index variable */
    int ret;            /* Return status */
    unsigned major, minor, release;     /* Library release versions */

#if defined H5_USE_114_API_DEFAULT || H5_USE_112_API_DEFAULT
    H5R_ref_t *wbuf;    /* buffer to write to disk          */
#else
    hobj_ref_t *wbuf;
#endif

    /* Get the library release versions */
    H5get_libversion(&major, &minor, &release);

    /* Open the test file */
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, H5P_DEFAULT);

    /* Create dataspace */
    sid = H5Screate_simple(1, dims1, NULL);

#if defined H5_USE_114_API_DEFAULT || defined H5_USE_112_API_DEFAULT

    wbuf = calloc(sizeof(H5R_ref_t), 2);

    /* Create a dataset with revised reference type */
    did = H5Dcreate2(fid, "Add_revised_ref_object", H5T_STD_REF, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* Create reference to /Group/Dataset */
    ret = H5Rcreate_object(fid, "/Group/Dataset", H5P_DEFAULT, &wbuf[0]);

    /* Create reference to /Group */
    ret = H5Rcreate_object(fid, "/Group", H5P_DEFAULT, &wbuf[1]);

    /* Write to the referenced dataset */
    ret = H5Dwrite(did, H5T_STD_REF, H5S_ALL, H5S_ALL, H5P_DEFAULT, wbuf);

    /* Destroy references */
   ret = H5Rdestroy(&wbuf[0]);
   ret = H5Rdestroy(&wbuf[1]);

#else

    wbuf = calloc(MAX(sizeof(unsigned), sizeof(hobj_ref_t)), 2);

    /* Create a dataset with old reference type */
    did = H5Dcreate1(fid, "Add_old_ref_object", H5T_STD_REF_OBJ, sid, H5P_DEFAULT);

    /* Create reference to /Group/Dataset */
    ret = H5Rcreate(&wbuf[0], fid, "/Group/Dataset", H5R_OBJECT, -1);

    /* Create reference to /Group */
    ret = H5Rcreate(&wbuf[1], fid, "/Group", H5R_OBJECT, -1);

    /* Write to the referenced dataset */
    ret = H5Dwrite(did, H5T_STD_REF_OBJ, H5S_ALL, H5S_ALL, H5P_DEFAULT, wbuf);

#endif

    /* Closing */
    H5Dclose(did);
    H5Sclose(sid);
    H5Fclose(fid);

    /* Free the buffer */
    if(wbuf) free(wbuf);

    return 0;
}
