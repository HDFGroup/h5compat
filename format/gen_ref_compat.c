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
 * This will create two datasets using the old reference API:
 *      --"Old_ref_object":  a dataset with object reference type
 *      --"Old_ref_region":  a dataset with region reference type
 */

#include "hdf5.h"
#include <stdlib.h>

/* The test file */
#define FILENAME    "ref_compat.h5"

/* Determine the maximum value */
#define MAX(a,b) (((a)>(b)) ? (a) : (b))

int main(void)
{
    hsize_t ref_dims[] = {2};
    hsize_t dset_dims[] = {100};
    hid_t fid = -1;         /* File ID */
    hid_t gid = -1;         /* Group ID */
    hid_t ref_sid = -1;     /* Dataspace ID */
    hid_t dset_sid = -1;    /* Dataspace ID */
    hid_t did1 = -1, did2 = -1, did3 = -1;     /* Dataset IDs */
    hsize_t start[1];       /* Starting location of hyperslab */
    hsize_t stride[1];      /* Stride of hyperslab */
    hsize_t count[1];       /* Element count of hyperslab */
    hsize_t block[1];       /* Block size of hyperslab */
    hsize_t coord1[10][1];  /* Coordinates for point selection */
    hobj_ref_t *obj_wbuf;
    hdset_reg_ref_t *reg_wbuf;  /* buffer to write to referenced data */
    unsigned *dwbuf;         /* Buffer for writing numeric data */
    unsigned i;                  /* Local index variable */
    herr_t ret;             /* Return status */

    dwbuf = calloc(sizeof(unsigned), 100);
    obj_wbuf = calloc(MAX(sizeof(unsigned), sizeof(hobj_ref_t)), 2);
    reg_wbuf = calloc(sizeof(hdset_reg_ref_t), 2);

    /* Create the test file */
    fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Create the dataspace */
    dset_sid = H5Screate_simple(1, dset_dims, NULL);

    /* Create a group */
    gid = H5Gcreate(fid, "Group", 0);

    /* Create a dataset in the group */
    did1 = H5Dcreate(gid, "Dataset", H5T_NATIVE_UINT, dset_sid, H5P_DEFAULT);

    /* Initialize data to write */
    for(i = 0; i < 100; i++)
        dwbuf[i] = i * 3;

    /* Write selection to disk */
    ret = H5Dwrite(did1, H5T_NATIVE_UINT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dwbuf);

    /* Close dataset */
    ret = H5Dclose(did1);

    /* Close group */
    ret = H5Gclose(gid);

     /* Create dataspace for the reference dataset */
    ref_sid = H5Screate_simple(1, ref_dims, NULL);

    /* Create a dataset with the old object reference type */
    did2 = H5Dcreate(fid, "Old_ref_object", H5T_STD_REF_OBJ, ref_sid, H5P_DEFAULT);

    /* Create reference to /Group/Dataset */
    ret = H5Rcreate(&obj_wbuf[0], fid, "/Group/Dataset", H5R_OBJECT, -1);

    /* Create reference to /Group */
    ret = H5Rcreate(&obj_wbuf[1], fid, "/Group", H5R_OBJECT, -1);

    /* Write to the referenced dataset */
    ret = H5Dwrite(did2, H5T_STD_REF_OBJ, H5S_ALL, H5S_ALL, H5P_DEFAULT, obj_wbuf);

    ret = H5Dclose(did2);




    /* Create a dataset with the old dataset region reference type */
    did3 = H5Dcreate(fid, "Old_ref_region", H5T_STD_REF_DSETREG, ref_sid, H5P_DEFAULT);

    /* Create references */

    /* Select 15 2x1 hyperslabs for first reference */
    start[0] = 2;
    stride[0] = 5;
    count[0] = 15;
    block[0] = 2;
    ret = H5Sselect_hyperslab(dset_sid, H5S_SELECT_SET, start, stride, count, block);

    /* Store first dataset region */
    ret = H5Rcreate(&reg_wbuf[0], fid, "/Group/Dataset", H5R_DATASET_REGION, dset_sid);

    /* Select sequence of ten points for second reference */
    coord1[0][0] = 16;
    coord1[1][0] = 22;
    coord1[2][0] = 38;
    coord1[3][0] = 41;
    coord1[4][0] = 52;
    coord1[5][0] = 63;
    coord1[6][0] = 70;
    coord1[7][0] = 89;
    coord1[8][0] = 97;
    coord1[9][0] = 03;
    ret = H5Sselect_elements(dset_sid, H5S_SELECT_SET, (size_t)10, (const hsize_t *)coord1);

    /* Store second dataset region */
    ret = H5Rcreate(&reg_wbuf[1], fid, "/Group/Dataset", H5R_DATASET_REGION, dset_sid);

    /* Write to the referenced dataset */
    ret = H5Dwrite(did3, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, reg_wbuf);

    /* Close Dataset */
    ret = H5Dclose(did3);

    /* Close dataspace */
    ret = H5Sclose(ref_sid);

    /* Close dataspace */
    ret = H5Sclose(dset_sid);

    /* Close file */
    ret = H5Fclose(fid);

    if(obj_wbuf) free(obj_wbuf);
    if(reg_wbuf) free(reg_wbuf);
    if(dwbuf) free(dwbuf);

    return 0;

} /* main() */
