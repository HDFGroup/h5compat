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

#define FILENAME        "compat_h5o.h5"
#define GROUPNAME       "Group1"
#define DSET_NAME       "/Dataset"

#if H5_VERS_MINOR > 8

typedef struct {
    const char *path;           /* Path to object */
    H5O_type_t type;            /* Type of object */
} obj_visit_t;

typedef struct {
    unsigned idx;               /* Index in object visit structure */
    const obj_visit_t *info;    /* Pointer to the object visit structure to use */
} ovisit_ud_t;
static const obj_visit_t ovisit0[] = {
    {".", H5O_TYPE_GROUP},
    {"Dataset", H5O_TYPE_DATASET},
    {"Group1", H5O_TYPE_GROUP}
}; 
/*------------------------------------------------------------------------ Function:    visit_obj_cb
 *
 * Purpose:     Callback routine for visiting objects in a file
 *
 * Return:      Success:        0
 *              Failure:        -1
 *
 * Programmer:  Quincey Koziol
 *              Sunday, November 25, 2007
 *
 *-------------------------------------------------------------------------
 */
static int
visit_obj_cb(hid_t group_id, const char *name, 
             const H5O_info_t *oinfo, void *_op_data)
{
    ovisit_ud_t *op_data = (ovisit_ud_t *)_op_data;

    /* Check for correct object information */
    if(strcmp(op_data->info[op_data->idx].path, name)) return(H5_ITER_ERROR);
    if(op_data->info[op_data->idx].type != oinfo->type) return(H5_ITER_ERROR);

    /* Advance to next location in expected output */
    op_data->idx++;

    return(H5_ITER_CONT);
} /* end visit_obj_cb() */
#endif /* H5_VERS_MINOR > 8 */


int
main(int argc, const char *argv[])
{

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* v1.8 and before didn't have the H5O* functions to be versioned in HDF5 1.12 */
#if H5_VERS_MINOR > 8
    hid_t	fid;		/* HDF5 File ID	*/
    hid_t       gid;            /* Group ID     */
    hid_t       sid;            /* Dataspace ID */
    hid_t       dsid;           /* Dataset ID   */
    H5O_info_t  infobuf;
    ovisit_ud_t udata;          /* User-data for visiting */

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR > 10
    printf("H5Oget_info_vers = %d\n", H5Oget_info_vers);
    printf("H5Oget_info_by_name_vers = %d\n", H5Oget_info_by_name_vers);
    printf("H5Oget_info_by_idx_vers = %d\n", H5Oget_info_by_idx_vers);
    printf("H5Ovisit_vers = %d\n", H5Ovisit_vers);
    printf("H5Ovisit_by_name_vers = %d\n", H5Ovisit_by_name_vers);
#endif /* H5_VERS_MINOR > 10 */

    /* Create a file with some ojects for testing the versioned H5O functions */
    if((fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;

    /* Create group */
#if defined(H5Gcreate_vers) && H5Gcreate_vers > 1
    if((gid = H5Gcreate(fid, GROUPNAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0) goto error;
#else
    if((gid = H5Gcreate(fid, GROUPNAME, (size_t)0)) < 0) goto error;
#endif

    /* Close group */
    if(H5Gclose(gid) < 0) goto error;

    /* Create dataspace for dataset */
    if((sid = H5Screate(H5S_SCALAR)) < 0) goto error;

    /* Create a dataset */
    if((dsid = H5Dcreate(fid, DSET_NAME, H5T_NATIVE_UCHAR, sid, H5P_DEFAULT)) < 0) goto error;    
    
    /* Close dataspace */
    if(H5Sclose(sid) < 0) goto error;

    /* Close dataset */
    if(H5Dclose(dsid) < 0) goto error;

#if defined(H5Oget_info_vers) && H5Oget_info_vers > 1
    if((H5Oget_info (fid, &infobuf, H5O_INFO_ALL)) < 0) goto error;
#else /* H5Oget_info_vers */
    if((H5Oget_info (fid, &infobuf)) < 0) goto error;
#endif /* H5Oget_info_vers */

#if defined(H5Oget_info_by_name_vers) && H5Oget_info_by_name_vers > 1
    if((H5Oget_info_by_name (fid, DSET_NAME, &infobuf, H5O_INFO_ALL, H5P_DEFAULT)) < 0) 
        goto error;
#else /* H5Oget_info_by_name_vers */
    if((H5Oget_info_by_name (fid, DSET_NAME, &infobuf, H5P_DEFAULT)) < 0) goto error;
#endif /* H5Oget_info_by_name_vers */

#if defined(H5Oget_info_by_idx_vers) && H5Oget_info_by_idx_vers > 1
    if((H5Oget_info_by_idx (fid, ".", H5_INDEX_NAME, H5_ITER_INC, (hsize_t)1,
                            &infobuf, H5O_INFO_ALL, H5P_DEFAULT)) < 0) goto error;
#else /* H5Oget_info_by_idx_vers */
    if((H5Oget_info_by_idx (fid, ".", H5_INDEX_NAME, H5_ITER_INC, (hsize_t)1, 
                            &infobuf, H5P_DEFAULT)) < 0) goto error;
#endif /* H5Oget_info_by_idx_vers */

#if defined(H5Ovisit_vers) && H5Ovisit_vers > 1
    udata.idx = 0;
    udata.info = ovisit0;
    if((H5Ovisit (fid, H5_INDEX_NAME, H5_ITER_INC, visit_obj_cb, 
                  &udata, H5O_INFO_BASIC)) < 0) goto error;
#else /* H5Ovisit_vers */
    udata.idx = 0;
    udata.info = ovisit0;
    if((H5Ovisit (fid, H5_INDEX_NAME, H5_ITER_INC, visit_obj_cb,
                  &udata)) < 0) goto error;
#endif /* H5Ovisit_vers */

#if defined(H5Ovisit_by_name_vers) && H5Ovisit_by_name_vers > 1
    udata.idx = 0;
    udata.info = ovisit0;
    if((H5Ovisit_by_name (fid, ".", H5_INDEX_NAME, H5_ITER_INC, visit_obj_cb, 
                          &udata, H5O_INFO_BASIC, H5P_DEFAULT)) < 0) goto error;
#else /* H5Ovisit_by_name_vers */
    udata.idx = 0;
    udata.info = ovisit0;
    if((H5Ovisit_by_name (fid, ".", H5_INDEX_NAME, H5_ITER_INC, visit_obj_cb,
                          &udata, H5P_DEFAULT)) < 0) goto error;
#endif /* H5Ovisit_by_name_vers */

    /* Close file */
    if(H5Fclose(fid) < 0) goto error;

#endif /* H5_VERS_MINOR > 8 */

    return(0);

error:
    return(1);
}

