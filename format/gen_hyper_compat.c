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
 *      h5cc gen_hyper_compat.c
 *
 *  Generate a test file with dataset region references for testing the
 *  hyperslab/point selection format change.
 *  See tests/t_hyper.c and read_hyper_compat.c.
 */

#include <stdlib.h>
#include "hdf5.h"

/* The test file */
#define FILENAME            "hyper_compat.h5"
#define POWER32             4294967296      /* 2^32 */

/* Datasets added */
#define DSET                "dset"
#define REF_NX_REG_DSET     "ref_nx32_reg_dset"
#define REF_NX_IRR_DSET     "ref_nx32_irr_dset"
#define REF_NX_PT_DSET      "ref_nx32_pt_dset"

int
main(int ac, char **av)
{
    hid_t fid = -1;         /* File ID */
    hid_t sid = -1;         /* Dataspace ID */
    hid_t did = -1;         /* Dataset ID */
    hid_t ref_sid = -1;     /* Dataset ID */
    hid_t ref_did1 = -1;    /* Dataset ID */
    hid_t ref_did2 = -1;    /* Dataset ID */
    hid_t ref_did3 = -1;    /* Dataset ID */
    hsize_t ref_dims[1];    /* Dimension size */
    hdset_reg_ref_t *ref_wbuf;  /* Buffer for storing the reference */
    hsize_t ref_start;      /* Starting location of hyperslab */
    hsize_t ref_stride;     /* Stride of hyperslab */
    hsize_t ref_count;      /* Element count of hyperslab */
    hsize_t ref_block;      /* Block size of hyperslab */
    hsize_t coord[4];       /* Point coordinates */
    hsize_t numparticles = 8388608;
    hsize_t total_particles = numparticles * 513;
    hsize_t vdsdims[1] = {total_particles};     /* Dataset dimension size */
    int ret;                /* Return status */

    ref_wbuf = calloc(sizeof(hdset_reg_ref_t), (size_t)1);

    /* Create the test file */
    fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Create a dataset */
    sid = H5Screate_simple(1, vdsdims, NULL);
    did = H5Dcreate2(fid, DSET, H5T_NATIVE_UINT, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* Create a dataset with dataset region reference */
    ref_dims[0] = 1;
    ref_sid = H5Screate_simple(1, ref_dims, NULL);
    ref_did1  = H5Dcreate2(fid, REF_NX_REG_DSET, H5T_STD_REF_DSETREG, ref_sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* Generate regular hyperslab not exceeding 32 */
    ref_start = 0;
    ref_count = 2;
    ref_block = 4;
    ref_stride = POWER32 - 4;
    ret = H5Sselect_hyperslab(sid, H5S_SELECT_SET, &ref_start, &ref_stride, &ref_count, &ref_block);
    
    /* Create the reference */
    ret = H5Rcreate(&ref_wbuf[0], fid, DSET, H5R_DATASET_REGION, sid);

    /* Write the reference to the referenced dataset */
    ret = H5Dwrite(ref_did1, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_wbuf);

    /* Generate irregular hyperslab not exceeding 32 */
    ref_start = 8;
    ref_count = 5;
    ref_block = 2;
    ref_stride = POWER32/5;
    ret = H5Sselect_hyperslab(sid, H5S_SELECT_OR, &ref_start, &ref_stride, &ref_count, &ref_block);

    /* Create the reference */
    ret = H5Rcreate(&ref_wbuf[0], fid, DSET, H5R_DATASET_REGION, sid);

    /* Create a dataset with dataset region reference */
    ref_did2  = H5Dcreate2(fid, REF_NX_IRR_DSET, H5T_STD_REF_DSETREG, ref_sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* Write the reference to the referenced dataset */
    ret = H5Dwrite(ref_did2, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_wbuf);


    /* Generate point selection not exceeding 32 */
    coord[0] = 5;
    coord[1] = 15;
    coord[2] = POWER32 - 1;
    coord[3] = 19;
    ret = H5Sselect_elements(sid, H5S_SELECT_SET, (size_t)4, coord);

    /* Create the reference */
    ret = H5Rcreate(&ref_wbuf[0], fid, "/dset", H5R_DATASET_REGION, sid);

    /* Create a dataset with dataset region reference */
    ref_did3  = H5Dcreate2(fid, REF_NX_PT_DSET, H5T_STD_REF_DSETREG, ref_sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* Write the reference to the referenced dataset */
    ret = H5Dwrite(ref_did3, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_wbuf);

    /* Closing */
    H5Dclose(ref_did1);
    H5Dclose(ref_did2);
    H5Dclose(ref_did3);
    H5Sclose(ref_sid);
    H5Dclose(did);
    H5Sclose(sid);
    H5Fclose(fid);
}

