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

#if H5_VERS_MINOR >= 8
#define ROOT_CLASS      H5P_ROOT
#else /* H5_VERS_MINOR >= 8 */
#define ROOT_CLASS      H5P_NO_CLASS
#endif /* H5_VERS_MINOR >= 8 */

/* Property definitions */
#define CLASS_NAME     "Class"

/* Property definitions */
#define PROP1_NAME     "Property 1"
int     prop1_def = 10;  /* Property default value */
#define PROP1_SIZE       sizeof(prop1_def)
#define PROP1_DEF_VALUE  (&prop1_def)

#define PROP2_NAME     "Property 2"
int     prop2_def = 20;  /* Property default value */
#define PROP2_SIZE       sizeof(prop2_def)
#define PROP2_DEF_VALUE  (&prop2_def)

/*
 * Basic tests of generic property (H5P) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

int
main(int argc, const char *argv[])
{
    hid_t	dcpl;		/* Dataset creation property list ID */
    hid_t	cid;		/* Generic Property class ID */
    hid_t	lid;		/* Generic Property list ID */
    size_t	nprops;		/* Number of properties */
    size_t      nalloc;         /* Number of bytes to be allocated for 
                                   buffer for encoded property list */

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 8
    printf("H5Pget_filter_vers = %d\n", H5Pget_filter_vers);
    printf("H5Pget_filter_by_id_vers = %d\n", H5Pget_filter_by_id_vers);
    printf("H5Pinsert_vers = %d\n", H5Pinsert_vers);
    printf("H5Pregister_vers = %d\n", H5Pregister_vers);
#endif /* H5_VERS_MINOR >= 8 */

#if H5_VERS_MINOR > 10
    printf("H5Pencode_vers = %d\n", H5Pencode_vers);
#endif /* H5_VERS_MINOR > 10 */


    /* Create a new generic class, derived from the root of the class hierarchy */
    if((cid = H5Pcreate_class(ROOT_CLASS, CLASS_NAME, NULL, NULL, NULL, NULL, NULL, NULL)) < 0) goto error;

    /* Insert first property into class (with no callbacks) */
#if defined(H5Pregister_vers) && H5Pregister_vers > 1
    if(H5Pregister(cid, PROP1_NAME, PROP1_SIZE, PROP1_DEF_VALUE, NULL, NULL, NULL, NULL, NULL, NULL, NULL) < 0) goto error;
#else /* H5Pregister_vers */
    if(H5Pregister(cid, PROP1_NAME, PROP1_SIZE, PROP1_DEF_VALUE, NULL, NULL, NULL, NULL, NULL, NULL) < 0) goto error;
#endif /* H5Pregister_vers */

    /* Check the existance of the property */
    if(H5Pexist(cid, PROP1_NAME) != 1) goto error;

    /* Check the number of properties in class */
    if(H5Pget_nprops(cid, &nprops) < 0) goto error;
    if(nprops != 1) goto error;

    /* Create a property list from the class */
    if((lid = H5Pcreate(cid)) < 0) goto error;

    /* Insert first temporary property into class (with no callbacks) */
#if defined(H5Pinsert_vers) && H5Pinsert_vers > 1
    if(H5Pinsert(lid, PROP2_NAME, PROP2_SIZE, PROP2_DEF_VALUE, NULL, NULL, NULL, NULL, NULL, NULL) < 0) goto error;
#else /* H5Pinsert_vers */
    if(H5Pinsert(lid, PROP2_NAME, PROP2_SIZE, PROP2_DEF_VALUE, NULL, NULL, NULL, NULL, NULL) < 0) goto error;
#endif /* H5Pinsert_vers */

    /* Check the existance of the property */
    if(H5Pexist(lid, PROP2_NAME) != 1) goto error;

    /* Check the number of properties in list */
    if(H5Pget_nprops(lid, &nprops) < 0) goto error;
    if(nprops != 2) goto error;

    /* Close list */
    if(H5Pclose(lid) < 0) goto error;

    /* Close class */
    if(H5Pclose_class(cid) < 0) goto error;


    /* Create dataset creation property list */
    if((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0) goto error;

#if defined H5_HAVE_FILTER_DEFLATE
{
    H5Z_filter_t filtn;                 /* filter identification number */
    size_t cd_nelmts = 1;               /* Number of filter parameters */
    unsigned cd_value;                  /* Filter parameter */

    if(H5Pset_deflate(dcpl, 6) < 0) goto error;

    /* Check for the deflate filter */
#if defined(H5Pget_filter_vers) && H5Pget_filter_vers > 1
    filtn = H5Pget_filter(dcpl, (unsigned)0, NULL, &cd_nelmts, &cd_value, (size_t)0, NULL, NULL);
#else /* H5Pget_filter_vers */
    filtn = H5Pget_filter(dcpl, (unsigned)0, NULL, &cd_nelmts, &cd_value, (size_t)0, NULL);
#endif /* H5Pget_filter_vers */
    if(H5Z_FILTER_DEFLATE != filtn)
        goto error;
    if(1 != cd_nelmts)
        goto error;
    if(6 != cd_value)
        goto error;

    /* Check for the deflate filter */
#if defined(H5Pget_filter_by_id_vers) && H5Pget_filter_by_id_vers > 1
    filtn = H5Pget_filter_by_id(dcpl, H5Z_FILTER_DEFLATE, NULL, &cd_nelmts, &cd_value, (size_t)0, NULL, NULL);
#else /* H5Pget_filter_by_id_vers */
    filtn = H5Pget_filter_by_id(dcpl, H5Z_FILTER_DEFLATE, NULL, &cd_nelmts, &cd_value, (size_t)0, NULL);
#endif /* H5Pget_filter_by_id_vers */
    if(1 != cd_nelmts)
        goto error;
    if(6 != cd_value)
        goto error;
}
#endif /* H5_HAVE_FILTER_DEFLATE */

#if H5_VERS_MINOR > 8
/* encode property list */
#if defined(H5Pencode_vers) && H5Pencode_vers > 1
    if((H5Pencode(dcpl, NULL, &nalloc, H5P_DEFAULT)) < 0 )
        goto error;
#else
    if((H5Pencode(dcpl, NULL, &nalloc)) < 0 )
        goto error;
#endif /* H5Pencode_vers */
#endif /* H5_VERS_MINOR > 8 */

    /* Close dataset creation property list */
    if(H5Pclose(dcpl) < 0) goto error;

    return(0);

error:
    return(1);
}

