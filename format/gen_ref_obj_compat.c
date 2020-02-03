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
 * This can be compiled on all library release versions with:
 *      h5cc -DH5_USE_16_API gen_ref_obj_compat.c
 *
 * This will create a referenced dataset using the old API.
 */

#include "hdf5.h"
#include <stdlib.h>

/* The test file */
#define FILENAME    "ref_obj_compat.h5"

/* Determine the maximum value */
#define MAX(a,b)    (((a)>(b)) ? (a) : (b))

int main(void)
{
    hobj_ref_t *wbuf;       /* Buffer to write to disk */
    hsize_t dims1[] = {10}; /* Dimension size */
    hid_t fid = -1;         /* File ID */
    hid_t gid = -1;         /* Group ID */
    hid_t sid = -1;         /* Dataspace ID */
    hid_t did1 = -1, did2 = -1;     /* Dataset IDs */
    unsigned *tu32;         /* Temporary pointer to uint32 data */
    int i;                  /* Local index variable */
    herr_t ret;             /* Return status */

    /* Allocate write & read buffers */
    wbuf = malloc(MAX(sizeof(unsigned), sizeof(hobj_ref_t)) * 10);

    /* Create the test file */
    fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Create the dataspace */
    sid = H5Screate_simple(1, dims1, NULL);

    /* Create a group */
    gid = H5Gcreate(fid, "Group", 0);

    /* Create a dataset in the group */
    did1 = H5Dcreate(gid, "Dataset", H5T_NATIVE_UINT, sid, H5P_DEFAULT);

    /* Initialize data to write */
    for(tu32 = (unsigned *)wbuf, i = 0; i <  10; i++)
        *tu32++= i * 3;

    /* Write selection to disk */
    ret = H5Dwrite(did1, H5T_NATIVE_UINT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wbuf);

    /* Close dataset */
    ret = H5Dclose(did1);

    /* Close group */
    ret = H5Gclose(gid);

    /* Create a dataset with the old object reference type */
    did2 = H5Dcreate(fid, "Old_ref_dset", H5T_STD_REF_OBJ, sid, H5P_DEFAULT);

    /* Create reference to /Group/Dataset */
    ret = H5Rcreate(&wbuf[0], fid, "/Group/Dataset", H5R_OBJECT, -1);

    /* Create reference to /Group */
    ret = H5Rcreate(&wbuf[1], fid, "/Group", H5R_OBJECT, -1);

    /* Write to the referenced dataset */
    ret = H5Dwrite(did2, H5T_STD_REF_OBJ, H5S_ALL, H5S_ALL, H5P_DEFAULT, wbuf);

    /* Close dataspace */
    ret = H5Sclose(sid);

    /* Close dataset */
    ret = H5Dclose(did2);

    /* Close file */
    ret = H5Fclose(fid);

    return 0;

} /* main() */
