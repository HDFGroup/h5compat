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

#define FILENAME        "compat_h5r.h5"
#define SPACE_RANK	1
#define SPACE_DIM	4

/*
 * Basic tests of reference (H5R) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

int
main(int argc, const char *argv[])
{
    hid_t	fid;		/* HDF5 File ID			*/
    hid_t	dsid;	        /* Dataset ID			*/
    hid_t	sid;            /* Dataspace ID			*/
    hsize_t	dims[] = {SPACE_DIM};
    hobj_ref_t  wbuf[SPACE_DIM];       /* Buffer to write to disk */
#if defined(H5Rget_obj_type_vers) && H5Rget_obj_type_vers > 1
    H5O_type_t  obj_type;       /* Object type */
#else /* H5Rget_obj_type */
    H5G_obj_t   obj_type;       /* Object type */
#endif /* H5Rget_obj_type */

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 8
    printf("H5Rget_obj_type_vers = %d\n", H5Rget_obj_type_vers);
#endif /* H5_VERS_MINOR >= 8 */

    /* Create file */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;

    /* Create dataspace for datasets */
    if((sid = H5Screate_simple(SPACE_RANK, dims, NULL)) < 0) goto error;

    /* Create datasets to reference */
    if((dsid = H5Dcreate(fid, "/Dataset1", H5T_NATIVE_UINT, sid, H5P_DEFAULT)) < 0) goto error;
    if(H5Dclose(dsid) < 0) goto error;

    if((dsid = H5Dcreate(fid, "/Dataset2", H5T_NATIVE_UINT, sid, H5P_DEFAULT)) < 0) goto error;
    if(H5Dclose(dsid) < 0) goto error;

    if((dsid = H5Dcreate(fid, "/Dataset3", H5T_NATIVE_UINT, sid, H5P_DEFAULT)) < 0) goto error;
    if(H5Dclose(dsid) < 0) goto error;

    if((dsid = H5Dcreate(fid, "/Dataset4", H5T_NATIVE_UINT, sid, H5P_DEFAULT)) < 0) goto error;
    if(H5Dclose(dsid) < 0) goto error;

    /* Create references to earlier datasets */
    if(H5Rcreate(&wbuf[0], fid, "/Dataset1", H5R_OBJECT, -1) < 0) goto error;
    if(H5Rcreate(&wbuf[1], fid, "/Dataset2", H5R_OBJECT, -1) < 0) goto error;
    if(H5Rcreate(&wbuf[2], fid, "/Dataset3", H5R_OBJECT, -1) < 0) goto error;
    if(H5Rcreate(&wbuf[3], fid, "/Dataset4", H5R_OBJECT, -1) < 0) goto error;

    /* Check type of references */
#if defined(H5Rget_obj_type_vers) && H5Rget_obj_type_vers > 1
    if(H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[0], &obj_type) < 0) goto error;
    if(obj_type != H5O_TYPE_DATASET) goto error;
    if(H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[1], &obj_type) < 0) goto error;
    if(obj_type != H5O_TYPE_DATASET) goto error;
    if(H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[2], &obj_type) < 0) goto error;
    if(obj_type != H5O_TYPE_DATASET) goto error;
    if(H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[3], &obj_type) < 0) goto error;
    if(obj_type != H5O_TYPE_DATASET) goto error;
#else /* H5Rget_obj_type_vers */
    if((obj_type = H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[0])) < 0) goto error;
    if(obj_type != H5G_DATASET) goto error;
    if((obj_type = H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[1])) < 0) goto error;
    if(obj_type != H5G_DATASET) goto error;
    if((obj_type = H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[2])) < 0) goto error;
    if(obj_type != H5G_DATASET) goto error;
    if((obj_type = H5Rget_obj_type(fid, H5R_OBJECT, &wbuf[3])) < 0) goto error;
    if(obj_type != H5G_DATASET) goto error;
#endif /* H5Rget_obj_type_vers */

    /* Close file */
    if(H5Fclose(fid) < 0) goto error;


    return(0);

error:
    return(1);
}

