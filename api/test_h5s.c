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

/* 3-D dataset with fixed dimensions */
#define SPACE1_RANK    3
#define SPACE1_DIM1    3
#define SPACE1_DIM2    15
#define SPACE1_DIM3    13


int
main(int argc, const char *argv[])
{
    hid_t	fid;		/* HDF5 File ID			*/

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* H5Sencode introduced after v 1.6 */
#if H5_VERS_MINOR > 6

    H5F_libver_t     low = H5F_LIBVER_EARLIEST;
    H5F_libver_t     high = H5F_LIBVER_LATEST;
    hid_t            sid;                 /* Dataspace ID */
    int              rank;                /* Logical rank of dataspace */
    hid_t            fapl = -1;           /* File access property list ID */
    hsize_t          dims1[] = {SPACE1_DIM1, SPACE1_DIM2, SPACE1_DIM3};
    size_t           sbuf_size=0, 
                     null_size=0, 
                     scalar_size=0;
    unsigned char   *sbuf=NULL, 
                    *null_sbuf=NULL, 
                    *scalar_buf=NULL;
    hsize_t          tdims[4];            /* Dimension array to test with */
    hssize_t         n;                   /* Number of dataspace elements */
    hsize_t          start[] = {0, 0, 0};
    hsize_t          stride[] = {2, 5, 3};
    hsize_t          count[] = {2, 2, 2};
    hsize_t          block[] = {1, 3, 1};

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR > 10
    printf("H5Sencode_vers = %d\n", H5Sencode_vers);
#endif /* H5_VERS_MINOR > 10 */


    /* Create a dataspace on which to call H5Sencode() */ 

    /* Create the file access property list */
    if((fapl = H5Pcreate(H5P_FILE_ACCESS)) < 0)
        goto error;

    /* Set low/high bounds in the fapl */
    if((H5Pset_libver_bounds(fapl, low, high)) < 0)
        goto error;

    /* Create the dataspace */
    if((sid = H5Screate_simple(SPACE1_RANK, dims1, NULL)) < 0)
        goto error;

    /* Set the hyperslab selection */
    if((H5Sselect_hyperslab(sid, H5S_SELECT_SET, start, stride, count, block)) < 0)
        goto error;
  
#if defined(H5Sencode_vers) && H5Sencode_vers > 1    
    /* Encode simple data space in a buffer with the fapl setting */
    if((H5Sencode(sid, NULL, &sbuf_size, fapl)) < 0)
        goto error;
#else
    if((H5Sencode(sid, NULL, &sbuf_size)) < 0)
        goto error;
#endif /* H5Sencode_vers */

    H5Sclose(sid);


#endif /* H5_VERS_MINOR > 6 */

    return(0);

error:
    return(1);
}

