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

#define FILENAME        "compat_h5t.h5"

/*
 * Basic tests of datatype (H5T) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

int
main(int argc, const char *argv[])
{
    hid_t	fid;		/* HDF5 File ID			*/
    hid_t	tid;            /* Datatype ID			*/

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 8
    printf("H5Tcommit_vers = %d\n", H5Tcommit_vers);
    printf("H5Topen_vers = %d\n", H5Topen_vers);
#endif /* H5_VERS_MINOR >= 8 */

    /* Create file */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;

    /* Copy a predefined datatype */
    if((tid = H5Tcopy(H5T_NATIVE_INT)) < 0) goto error;

    /* Commit the copied datatype */
#if defined(H5Tcommit_vers) && H5Tcommit_vers > 1
    if(H5Tcommit(fid, "native-int", tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT) < 0) goto error;
#else /* H5Tcommit_vers */
    if(H5Tcommit(fid, "native-int", tid) < 0) goto error;
#endif /* H5Tcommit_vers */

    /* Close datatype */
    if(H5Tclose(tid) < 0) goto error;

    /* Re-open the named datatype */
#if defined(H5Topen_vers) && H5Topen_vers > 1
    if((tid = H5Topen(fid, "native-int", H5P_DEFAULT)) < 0) goto error;
#else /* H5Topen_vers */
    if((tid = H5Topen(fid, "native-int")) < 0) goto error;
#endif /* H5Topen_vers */

    /* Close datatype */
    if(H5Tclose(tid) < 0) goto error;

    /* Close file */
    if(H5Fclose(fid) < 0) goto error;


    return(0);

error:
    return(1);
}

