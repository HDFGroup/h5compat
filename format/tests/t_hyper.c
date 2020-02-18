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
 *  This can be compiled on all library release versions (except v16) with:
 *      h5cc t_hyper.c
 *
 *  Add datasets with region references for testing the hyperslab/point 
 *  selection format change.
 *
 *  See gen_hyper_compat.c and read_hyper_compat.c.
 */

#include <stdlib.h>
#include <assert.h>
#include "hdf5.h"

#define POWER32     4294967296      /* 2^32 */

/* The test file */
#define FILENAME    "hyper_compat.h5"

/* The existing dataset */
#define DSET                "dset"

/* Datasets added */
#define ADD_REF_X_REG_DSET     "add_ref_x32_reg_dset"
#define ADD_REF_X_IRR_DSET     "add_ref_x32_irr_dset"
#define ADD_REF_X_PT_DSET      "add_ref_x32_pt_dset"

/* Library release versions */
#define LIB_REL_V110     10
#define LIB_REL_V112     12

int
main(int ac, char **av)
{
    hid_t fid = -1;
    hid_t sid = -1;
    hid_t did = -1;
    hid_t ref_sid = -1;;
    hid_t ref_did1 = -1; 
    hid_t ref_did2 = -1; 
    hid_t ref_did3 = -1;
    hsize_t ref_dims[1];
    hsize_t numparticles = 8388608;
    hsize_t total_particles = numparticles * 513;
    hsize_t vdsdims[1] = {total_particles};
    hsize_t coord[4];
    hdset_reg_ref_t *ref_wbuf;
    hsize_t ref_start;     /* Starting location of hyperslab */
    hsize_t ref_stride;    /* Stride of hyperslab */
    hsize_t ref_count;     /* Element count of hyperslab */
    hsize_t ref_block;     /* Block size of hyperslab */
    unsigned minor;         /* Library release versions */
    int ret;

    /* Get the library release version */
    H5get_libversion(NULL, &minor, NULL);

    ref_wbuf = calloc(sizeof(hdset_reg_ref_t), (size_t)1);

    /* Open the test file */
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, H5P_DEFAULT);

    /* Open the dataset */
    did = H5Dopen2(fid, DSET, H5P_DEFAULT);
    sid = H5Dget_space(did);

    /* Create dataspace for the referenced dataset */
    ref_dims[0] = 1;
    ref_sid = H5Screate_simple(1, ref_dims, NULL);

    /* Generate regular hyperslab exceeding 32 */
    ref_start = 0;
    ref_count = 2;
    ref_block = 4;
    ref_stride = POWER32;
    ret = H5Sselect_hyperslab(sid, H5S_SELECT_SET, &ref_start, &ref_stride, &ref_count, &ref_block);

    /* Create the first reference */
    H5E_BEGIN_TRY {
        ret = H5Rcreate(&ref_wbuf[0], fid, DSET, H5R_DATASET_REGION, sid);
    } H5E_END_TRY;

    /* Should succeed for v110 and above */
    if(minor >= LIB_REL_V110) {
        assert(ret >= 0);

        /* Create the referenced datset */
        ref_did1  = H5Dcreate2(fid, ADD_REF_X_REG_DSET, H5T_STD_REF_DSETREG, ref_sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        /* Write to the referenced datset */
        ret = H5Dwrite(ref_did1, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_wbuf);
    } else
        /* Should fail for v18 and below */
        assert(ret < 0);


    /* Generate irregular bhyperslab exceeding 32 */
    ref_start = 8;
    ref_count = 5;
    ref_block = 2;
    ref_stride = POWER32;
    ret = H5Sselect_hyperslab(sid, H5S_SELECT_OR, &ref_start, &ref_stride, &ref_count, &ref_block);

    /* Create the second reference */
    H5E_BEGIN_TRY {
        ret = H5Rcreate(&ref_wbuf[0], fid, "/dset", H5R_DATASET_REGION, sid);
    } H5E_END_TRY;

    /* Should succeed for v112 and above */
    if(minor >= LIB_REL_V112) {
        assert(ret >= 0);

        /* Create the referenced datset */
        ref_did2  = H5Dcreate2(fid, ADD_REF_X_IRR_DSET, H5T_STD_REF_DSETREG, ref_sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        /* Write to the referenced datset */
        ret = H5Dwrite(ref_did2, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_wbuf);
    } else
        /* Should fail for v110 and below */
        assert(ret < 0);

    /* Generate point selection exceeding 32 */
    coord[0] = 5;
    coord[1] = 15;
    coord[2] = POWER32 + 1;
    coord[3] = 19;
    ret = H5Sselect_elements(sid, H5S_SELECT_SET, (size_t)4, coord);

    /* Create the third reference */
    H5E_BEGIN_TRY {
        ret = H5Rcreate(&ref_wbuf[0], fid, "/dset", H5R_DATASET_REGION, sid);
    } H5E_END_TRY;

    /* Should succeed for v112 and above */
    if(minor >= LIB_REL_V112) {
        assert(ret >= 0);

        /* Create the referenced datset */
        ref_did3  = H5Dcreate2(fid, ADD_REF_X_PT_DSET, H5T_STD_REF_DSETREG, ref_sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        /* Write to the referenced datset */
        ret = H5Dwrite(ref_did3, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_wbuf);
    } else
        /* Should fail for v110 and below */
        assert(ret < 0);


    /* Closing */
    if(ref_did1 >= 0) 
        H5Dclose(ref_did1);
    if(ref_did2 >= 0) 
        H5Dclose(ref_did2);
    if(ref_did3 >= 0) 
        H5Dclose(ref_did3);

    if(did >= 0) 
        H5Dclose(did);

    if(ref_sid >= 0)
        H5Sclose(ref_sid);

    if(sid >= 0)
        H5Sclose(sid);

    if(fid >= 0)
        H5Fclose(fid);
} /* main () */
