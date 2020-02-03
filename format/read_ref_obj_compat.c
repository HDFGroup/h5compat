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
 *      h5cc -DH5_USE_16_API read_ref_obj_compat.c
 *
 *  This will try to read the referenced datasets in the test 
 *  file FILENAME and verify the expected result.
 */

#include "hdf5.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define FILEPATH "./errors.log"         /* The error log file */
#define FILENAME "ref_obj_compat.h5"    /* The test file */

/* Verify the returned value and print the given messages */
int check(int ret, const char *type, const char *name, const char *message, FILE *fp)
{
    if (ret < 0){
        fprintf(fp, "Error: %s %s %s\n", type, name, message ? message : "(null)");
    }
    else{
        fprintf(fp, "Passed: %s %s %s\n", type, name, message ? message : "(null)");
    }

    return ret;
}

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

    /* Try to open "Old_ref_dset" which is created by gen_ref_obj_compat.c */
    H5E_BEGIN_TRY {
        did = H5Dopen(fid, "Old_ref_dset");
    } H5E_END_TRY;
    if(did < 0)
        check(-1, "Opening dataset", "Old_ref_dset", NULL, fp);
    else {
        check(0, "Opening dataset", "Old_ref_dset", NULL, fp);
        H5Dclose(did);
    }

    /* 
     * Try to open "Add_old_ref_dset" or "Add_revised_ref_dset" 
     * which is added by tests/t_ref_obj.c.
     */

    H5E_BEGIN_TRY {
        did1 = H5Dopen(fid, "Add_old_ref_dset");
    } H5E_END_TRY;

    H5E_BEGIN_TRY {
        did2 = H5Dopen(fid, "Add_revised_ref_dset");
    } H5E_END_TRY;

    /* 
     * For library release v12 and above:
     *  -- should be able to open "Add_old_ref_dset" or 
     *      "Add_revised_ref_dset"
     * For library release v10 and below:
     *  --should be able to open "Add_old_ref_dset" or
     *  --should fail to open "Add_revised_ref_dset
     */
    if(minor >= 12) {
        if((did1 < 0 && did2 < 0) || (did1 >= 0 && did2 >= 0)) {
            check(-1, "Opening datasets", "Add_old_ref_dset/Add_revised_ref_dset", "Invalid state", fp);
        } else {
            if(did1 >= 0)
                check(0, "Opening dataset", "Add_old_ref_dset", NULL, fp);
            else if(did2 >=0)
                check(0, "Opening dataset", "Add_revised_ref_dset", NULL, fp);
        }
    } else {
        if((did1 >= 0 && did2 >= 0) || (did1 < 0 && did2 >= 0)) {
            check(-1, "Opening datasets", "Add_old_ref_dset/Add_revised_ref_dset", "Invalid state", fp);
        } else if(did1 >= 0)
            check(0, "Opening dataset", "Add_old_ref_dset", NULL, fp);
        else if(did2 < 0)
            check(0, "Opening dataset", "Add_revised_ref_dset", "Correct to fail in opening", fp);
    }

    /* Close the file */
    H5Fclose(fid);

    /* Close the error log file */
    fclose(fp);

    return 0;
}
