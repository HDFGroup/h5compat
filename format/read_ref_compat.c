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
 *      h5cc -DH5_USE_16_API read_ref_compat.c
 *
 *  This will try to read the referenced datasets in the test 
 *  file FILENAME and verify the expected result
 */

#include "hdf5.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define FILEPATH "./errors.log"     /* The error log file */
#define FILENAME "ref_compat.h5"    /* The test file */
#define LIB_REL_V12     12

/* Datasets created by gen_ref_compat.c */
#define OLD_OBJ_NAME   "Old_ref_object"
#define OLD_REG_NAME   "Old_ref_region"

/* Datasets added by tests/t_ref.c */
#define ADD_OLD_OBJ_NAME        "Add_old_ref_object"
#define ADD_REVISED_OBJ_NAME    "Add_revised_ref_object"

/* Datasets added by tests/t_ref_region.c */
#define ADD_OLD_REG_NAME        "Add_old_ref_region"
#define ADD_REVISED_REG_NAME    "Add_revised_ref_region"

/* Verify the returned value and print the given messages */
int
check(int ret, const char *type, const char *name, const char *message, FILE *fp)
{
    if(ret < 0)
        fprintf(fp, "Error: %s %s %s\n", type, name, message ? message : "(null)");
    else
        fprintf(fp, "Passed: %s %s %s\n", type, name, message ? message : "(null)");

    return ret;
} /* check() */

/* Verify the expected result from opening a dataset in the file */
int 
check_a_dset(hid_t did, const char *type, const char *name, const char *message, FILE *fp)
{
    if(did < 0)
        check(-1, "Opening dataset", name, NULL, fp);
    else {
        check(0, "Opening dataset", name, NULL, fp);
        H5Dclose(did);
    }

    return 0;

} /* check_a_dset() */

/* Verify the expected results from opening the datasets added to the file */
int 
check_dsets(unsigned rel_minor, hid_t did1, hid_t did2, const char *name1, const char *name2, FILE *fp)
{
    if(rel_minor >= LIB_REL_V12) {
        if(did1 < 0 && did2 < 0) 
            check(-1, "Opening datasets", "Invalid state", NULL, fp);
        else if (did1 >= 0 && did2 >= 0) {
            check(-1, "Opening datasets", "Invalid state", NULL, fp);
            H5Dclose(did1);
            H5Dclose(did2);
        } else {
            if(did1 >= 0) {
                check(0, "Opening dataset", name1, NULL, fp);
                H5Dclose(did1);
            } else if(did2 >=0) {
                check(0, "Opening dataset", name2, NULL, fp);
                H5Dclose(did2);
            }
        }
    } else {
        if(did1 >= 0 && did2 >= 0) {
            check(-1, "Opening datasets", "Invalid state", NULL, fp);
            H5Dclose(did1);
            H5Dclose(did2);
        } else if (did1 < 0 && did2 >= 0) {
            check(-1, "Opening datasets", "Invalid state", NULL, fp);
            H5Dclose(did2);
        } else if(did1 >= 0) {
            check(0, "Opening dataset", name1,  NULL, fp);
            H5Dclose(did1);
        } else if(did2 < 0)
            check(0, "Opening dataset", name2, "Correct to fail in opening", fp);
    }

    return 0;
} /* check_dsets() */


int main(void)
{
    FILE *fp;

    hid_t fid = -1;		/* File ID */
    hid_t did1 = -1;    /* Dataset ID */
    hid_t did2 = -1;    /* Dataset ID */
    hid_t did = -1;     /* Dataset ID */
    unsigned major, minor, release; /* Library release versions */

    /* Open the error log file */
    fp = fopen(FILEPATH, "a");

    /* Get library release version */
    H5get_libversion(&major, &minor, &release);

    /* Open the test file */
    fid = H5Fopen(FILENAME, H5F_ACC_RDONLY, H5P_DEFAULT);

    /* Try to open "Old_ref_object" which is created by gen_ref_compat.c */
    H5E_BEGIN_TRY {
        did = H5Dopen(fid, OLD_OBJ_NAME);
    } H5E_END_TRY;

    check_a_dset(did, "Opening dataset", OLD_OBJ_NAME, NULL, fp);

    /* Try to open "Old_ref_region" which is created by gen_ref_compat.c */
    H5E_BEGIN_TRY {
        did = H5Dopen(fid, OLD_REG_NAME);
    } H5E_END_TRY;

    check_a_dset(did, "Opening dataset", OLD_REG_NAME, NULL, fp);

    /* 
     * Try to open "Add_old_ref_object" or "Add_revised_ref_object" 
     * which is added by tests/t_ref_object.c.
     */
    H5E_BEGIN_TRY {
        did1 = H5Dopen(fid, ADD_OLD_OBJ_NAME);
    } H5E_END_TRY;

    H5E_BEGIN_TRY {
        did2 = H5Dopen(fid, ADD_REVISED_OBJ_NAME);
    } H5E_END_TRY;

    /* 
     * For library release v12 and above:
     *  --should be able to open 
     *      "Add_old_ref_object" 
     *          or 
     *      "Add_revised_ref_dset"
     *
     * For library release v10 and below:
     *  --should be able to open "Add_old_ref_object" 
     *      or
     *  --should fail to open "Add_revised_ref_object
     */
    check_dsets(minor, did1, did2, ADD_OLD_OBJ_NAME, ADD_REVISED_OBJ_NAME, fp);

    /* 
     * Try to open "Add_old_ref_region" or "Add_revised_ref_region" 
     * which is added by tests/t_ref_region.c.
     */
    H5E_BEGIN_TRY {
        did1 = H5Dopen(fid, ADD_OLD_REG_NAME);
    } H5E_END_TRY;

    H5E_BEGIN_TRY {
        did2 = H5Dopen(fid, ADD_REVISED_REG_NAME);
    } H5E_END_TRY;

    /* 
     * For library release v12 and above:
     *  --should be able to open 
     *      "Add_old_ref_region" 
     *          or 
     *      "Add_revised_ref_region:
     *
     * For library release v10 and below:
     *  --should be able to open "Add_old_ref_region"
     *     or
     *  --should fail to open "Add_revised_ref_region:
     */
    check_dsets(minor, did1, did2, ADD_OLD_REG_NAME, ADD_REVISED_REG_NAME, fp);

    /* Close the file */
    H5Fclose(fid);

    /* Close the error log file */
    fclose(fp);

    return 0;
} /* main() */
