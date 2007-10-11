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
#define PROP_NAME     "Property"
int     prop_def = 10;  /* Property default value */
#define PROP_SIZE       sizeof(prop_def)
#define PROP_DEF_VALUE  (&prop_def)

/*
 * Basic tests of generic property (H5P) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

int
main(int argc, const char *argv[])
{
    hid_t	cid;		/* Generic Property class ID */

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 8
    printf("H5Pregister_vers = %d\n", H5Pregister_vers);
#endif /* H5_VERS_MINOR >= 8 */

    /* Create a new generic class, derived from the root of the class hierarchy */
    if((cid = H5Pcreate_class(ROOT_CLASS, CLASS_NAME, NULL, NULL, NULL, NULL, NULL, NULL)) < 0) goto error;

    /* Insert first property into class (with no callbacks) */
#if defined(H5Pregister_vers) && H5Pregister_vers > 1
    if(H5Pregister(cid, PROP_NAME, PROP_SIZE, PROP_DEF_VALUE, NULL, NULL, NULL, NULL, NULL, NULL, NULL) < 0) goto error;
#else /* H5Pregister_vers */
    if(H5Pregister(cid, PROP_NAME, PROP_SIZE, PROP_DEF_VALUE, NULL, NULL, NULL, NULL, NULL, NULL) < 0) goto error;
#endif /* H5Pregister_vers */

    /* Check the existance of the property */
    if(H5Pexist(cid, PROP_NAME) != 1) goto error;

    /* Close class */
    if(H5Pclose_class(cid) < 0) goto error;

    return(0);

error:
    return(1);
}

