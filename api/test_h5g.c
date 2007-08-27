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

#define FILENAME        "compat_h5g.h5"
#define GROUPNAME       "Group1"

/*
 * Basic tests of group (H5G) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

int
main(int argc, const char *argv[])
{
    hid_t fid;          /* File ID */
    hid_t gid;          /* Group ID */

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Create file */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;

    /* Create group */
#if defined(H5Gcreate_vers) && H5Gcreate_vers > 1
    if((gid = H5Gcreate2(fid, GROUPNAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;
#else /* H5Gcreate_vers */
    if((gid = H5Gcreate(fid, GROUPNAME, (size_t)0)) < 0) goto error;
#endif /* H5Gcreate_vers */

    /* Close group */
    if(H5Gclose(gid) < 0) goto error;

    /* Close file */
    if(H5Fclose(fid) < 0) goto error;


    return(0);

error:
    return(1);
}

