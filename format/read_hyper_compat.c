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
 *      h5cc read_hyper_compat.c
 *
 *  Test the hyperslab/point selection format change which will be triggered
 *  when accessing the dataset region references via H5Rget_region().
 *
 *  See gen_hyper_compat.c and tests/t_hyper.c.
 */

#include "hdf5.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

/* The error log file */
#define FILEPATH    "./errors.log" 

/* The test file */
#define FILENAME    "hyper_compat.h5"    

/* Library release versions */
#define LIB_REL_V12     12
#define LIB_REL_V10     10

#define TRUE    1
#define FALSE   0

/* Datasets added in the test file by gen_hyper_compat.c */
#define REF_NX_REG_DSET     "ref_nx32_reg_dset"
#define REF_NX_IRR_DSET     "ref_nx32_irr_dset"
#define REF_NX_PT_DSET      "ref_nx32_pt_dset"

/* Datasets added in the test file by tests/t_hyper.c */
#define ADD_REF_X_REG_DSET     "add_ref_x32_reg_dset"
#define ADD_REF_X_IRR_DSET     "add_ref_x32_irr_dset"
#define ADD_REF_X_PT_DSET      "add_ref_x32_pt_dset"

/* Verify the returned value and print the given message */
int
check(int ret, const char *type, const char *name, const char *message, FILE *fp)
{
    if(ret < 0)
        fprintf(fp, "Error: %s %s %s\n", type, name, message ? message : "(null)");
    else
        fprintf(fp, "Passed: %s %s %s\n", type, name, message ? message : "(null)");

    return ret;
} /* check() */

/*
 *  Verify the dataspace selection in the referenced dataset is as expected.
 *
 *  Dataset:   REF_NX_REG_DSET, REF_NX_IRR_DSET, REF_NX_PT_DSET    
 *      For all library releases v18 and above:
 *      --The dataset exists and should be able to access the region reference in its dataspace
 *
 *  Dataset:    ADD_REF_X_REG_DSET
 *      For library releases v110 and above:
 *      --The dataset does not exist 
 *          or
 *        The dataset exists and should be able to access the region reference in its dataspace
 *
 *      For library release v18:
 *      --The dataset does not exist 
 *          or
 *        The dataset exists but fail to access the region reference in its dataspace
 *
 *   Dataset:   ADD_REF_X_IRR_DSET, ADD_REF_X_PT_DSET   
 *      For library release v112 and above:
 *      --The dataset does not exist 
 *          or
 *        The dataset exists and should be able to access the region reference in its dataspace
 *
 *      For library release v110 and v18:
 *      --The dataset does not exist 
 *          or
 *        The dataset exists but fail to access the region reference in its dataspace
*/
int
check_dataspace(hid_t fid, char *dname, unsigned in_rel, FILE *fp)
{
    hid_t sid = -1;             /* Dataspace ID */
    hid_t did = -1;             /* Dataset ID */
    hdset_reg_ref_t *ref_rbuf;  /* The buffer for the reference */
    H5S_sel_type sel_type;      /* Selection type */
    htri_t exists;              /* Existed or not */
    unsigned minor;             /* Library release versions */
    int ret;                    /* Return status */

    /* Get library release version */
    H5get_libversion(NULL, &minor, NULL);

    ref_rbuf = calloc(sizeof(hdset_reg_ref_t), 1);

    /* Check if the dataset existed */
    exists = H5Lexists(fid, dname, H5P_DEFAULT);
    if(exists == TRUE) {
        /* Open and read the referenced dataset */
        did = H5Dopen2(fid, dname, H5P_DEFAULT);
        ret = H5Dread(did, H5T_STD_REF_DSETREG, H5S_ALL, H5S_ALL, H5P_DEFAULT, ref_rbuf);

        /* Open access to the region reference from the dataset's dataspace */
        H5E_BEGIN_TRY {
            sid = H5Rget_region(did, H5R_DATASET_REGION, &ref_rbuf[0]);
        } H5E_END_TRY;
    }

    /* Verify the expected result */
    if(minor >= in_rel) {
        /* Does not exist or succeed to open the dataspace */
        assert(exists == FALSE || sid >= 0);
        if(exists == FALSE)
            check(0, "Dataset does not exist", dname, "Correct", fp);
        else if(sid >= 0) {
            htri_t ok = FALSE;

            sel_type = H5Sget_select_type(sid);

            if(!strcmp(dname, ADD_REF_X_PT_DSET) || !strcmp(dname, REF_NX_PT_DSET)) {
                assert(sel_type ==  H5S_SEL_POINTS);
                if(sel_type ==  H5S_SEL_POINTS)
                    ok = TRUE;
            } else {
                assert(sel_type ==  H5S_SEL_HYPERSLABS);
                if(sel_type ==  H5S_SEL_HYPERSLABS)
                    ok = TRUE;
            }

            if(ok)
                check(0, "Succeed in opening region reference and correct selection type", dname, NULL, fp);
            else
                check(-1, "Invalid state", dname, "Please check", fp);
        }
    } else {
        /*  not exists or fail to open */
        assert(exists == FALSE || sid < 0);
        if(exists == FALSE)
            check(0, "Dataset does not exist", dname, "Correct", fp);
        else if(sid < 0)
            check(0, "Fail to open dataset region reference", dname, "Correct", fp);
        else
            check(-1, "Invalid state", dname, "Please check", fp);
    }

    if(did >= 0) 
        H5Dclose(did);
    if(sid >= 0) 
        H5Sclose(sid);

    return(0);

} /* check_dataspace() */

int main(void)
{
    FILE *fp;           /* Pointer to log file */
    hid_t fid = -1;		/* File ID */

    /* Open the error log file */
    fp = fopen(FILEPATH, "a");

    /* Open the test file */
    fid = H5Fopen(FILENAME, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Check the expected dataspace */
    check_dataspace(fid, REF_NX_REG_DSET, 0, fp);
    check_dataspace(fid, REF_NX_IRR_DSET, 0, fp);
    check_dataspace(fid, REF_NX_PT_DSET, 0, fp);

    /* Check the expected dataspace */
    check_dataspace(fid, ADD_REF_X_REG_DSET, LIB_REL_V10, fp);
    check_dataspace(fid, ADD_REF_X_IRR_DSET, LIB_REL_V12, fp);
    check_dataspace(fid, ADD_REF_X_PT_DSET, LIB_REL_V12, fp);

    /* Close the file */
    H5Fclose(fid);

    /* Close the error log file */
    fclose(fp);

    return 0;
} /* main() */
