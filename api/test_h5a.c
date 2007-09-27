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

#define FILENAME        "compat_h5a.h5"
#define DSET_NAME       "/Dataset"
#define ATTR_NAME       "attr"
#define ATTR2_NAME      "attr2"

/*
 * Basic tests of attribute (H5A) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

int
main(int argc, const char *argv[])
{
    hid_t	fid;		/* HDF5 File ID			*/
    hid_t	sid;            /* Dataspace ID			*/
    hid_t	dsid;           /* Dataset ID			*/
    hid_t	aid;            /* Attribute ID			*/

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 8
    printf("H5Adelete_vers = %d\n", H5Adelete_vers);
    printf("H5Arename_vers = %d\n", H5Arename_vers);
#endif /* H5_VERS_MINOR >= 8 */

    /* Create file */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;

    /* Create dataspace for attribute and dataset */
    if((sid = H5Screate(H5S_SCALAR)) < 0) goto error;

    /* Create a dataset */
    if((dsid = H5Dcreate(fid, DSET_NAME, H5T_NATIVE_UCHAR, sid, H5P_DEFAULT)) < 0) goto error;


    /* Add attribute to dataset */

    /* Create attribute */
    if((aid = H5Acreate(dsid, ATTR_NAME, H5T_NATIVE_UINT, sid, H5P_DEFAULT)) < 0) goto error;

    /* Close dataspace */
    if(H5Sclose(sid) < 0) goto error;

    /* Close attribute */
    if(H5Aclose(aid) < 0) goto error;


    /* Rename the attribute */
#if defined(H5Arename_vers) && H5Arename_vers > 1
    if(H5Arename(dsid, ".", ATTR_NAME, ATTR2_NAME, H5P_DEFAULT) < 0) goto error;
#else /* H5Arename_vers */
    if(H5Arename(dsid, ATTR_NAME, ATTR2_NAME) < 0) goto error;
#endif /* H5Arename_vers */


    /* Delete the attribute */
#if defined(H5Adelete_vers) && H5Adelete_vers > 1
    if(H5Adelete(dsid, ".", ATTR2_NAME, H5P_DEFAULT) < 0) goto error;
#else /* H5Adelete_vers */
    if(H5Adelete(dsid, ATTR2_NAME) < 0) goto error;
#endif /* H5Adelete_vers */


    /* Close dataset */
    if(H5Dclose(dsid) < 0) goto error;

    /* Close file */
    if(H5Fclose(fid) < 0) goto error;


    return(0);

error:
    return(1);
}
