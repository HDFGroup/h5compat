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
 * Basic tests of dataset (H5F) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

#define FILENAME        "compat_h5f.h5"

int
main(int argc, const char *argv[])
{
    hid_t	fid;		/* HDF5 File ID			*/

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* v1.6 didn't didn't have H5Fget_info function or H5F_info_t structure. 
       Skip to end for version 1.6 or when using 1.6 APIs.  */
#if H5_VERS_MINOR > 6
    /* H5F_info_t was not versioned in v1.8 */
#if H5_VERS_MINOR == 8
    H5F_info_t file_info;
#elif defined(H5Fget_info_vers) && H5Fget_info_vers > 1
    H5F_info2_t file_info;
#else
    H5F_info1_t file_info;
#endif /* H5_VERS_MINOR == 8 */

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 10
    printf("H5Fget_info_vers = %d\n", H5Fget_info_vers);
#endif

    /* Create file */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;

    /* Get file info */
    if((H5Fget_info(fid, &file_info)) < 0) goto error;

    /* Close file */
    if(H5Fclose(fid) < 0) goto error;

#endif /* H5_VERS_MINOR > 6 */

    return(0);

error:
    return(1);
}

