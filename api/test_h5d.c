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

#include "hdf5.h"

/*
 * Basic tests of dataset (H5D) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

#define FILENAME        "compat_h5d.h5"
#define DSET_NAME       "/Dataset"

int
main(int argc, const char *argv[])
{
    hid_t	fid;		/* HDF5 File ID			*/
    hid_t	sid;            /* Dataspace ID			*/
    hid_t	dsid;           /* Dataset ID			*/

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 8
    printf("H5Dopen_vers = %d\n", H5Dopen_vers);
#endif /* H5_VERS_MINOR >= 8 */

    /* Create file */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;

    /* Create dataspace for attribute and dataset */
    if((sid = H5Screate(H5S_SCALAR)) < 0) goto error;

    /* Create a dataset */
    if((dsid = H5Dcreate(fid, DSET_NAME, H5T_NATIVE_UCHAR, sid, H5P_DEFAULT)) < 0) goto error;

    /* Close dataspace */
    if(H5Sclose(sid) < 0) goto error;

    /* Close dataset */
    if(H5Dclose(dsid) < 0) goto error;


    /* Open the dataset */
#if defined(H5Dopen_vers) && H5Dopen_vers > 1
    if((dsid = H5Dopen(fid, DSET_NAME, H5P_DEFAULT)) < 0) goto error;
#else /* H5Dopen_vers */
    if((dsid = H5Dopen(fid, DSET_NAME)) < 0) goto error;
#endif /* H5Dopen_vers */

    /* Close dataset */
    if(H5Dclose(dsid) < 0) goto error;

    /* Close file */
    if(H5Fclose(fid) < 0) goto error;


    return(0);

error:
    return(1);
}

