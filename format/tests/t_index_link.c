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

#define FILENAME "compat.h5"

int main(int argc, char *argv[])
{
/* =====  Variables  ===== */

    hid_t fid;		/* file ID */
    hid_t gid;          /* group ID */
    hid_t aid;          /* attribute ID */
    hid_t did;          /* dataset ID */
    hid_t sid;		/* dataspace ID */
    hid_t tid;   	/* datatype ID */
    hid_t gcpl;		/* group creation property list */

/* =====  Main processing  ===== */

   /* Open File */
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, H5P_DEFAULT);

   /* remove group g4 */
    H5Ldelete( fid, "g4", H5P_DEFAULT );

   /* Create Group g4 */
    gcpl = H5Pcreate(H5P_GROUP_CREATE);
    H5Pset_attr_creation_order( gcpl, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED );
    gid = H5Gcreate2(fid, "/g4", H5P_DEFAULT, gcpl, H5P_DEFAULT);

    H5Lcreate_hard( fid, "/g3/hlink2", gid, "dset2", H5P_DEFAULT, H5P_DEFAULT );
    
   /* Close Group */
    H5Gclose(gid);

   /* Close File */
    H5Fclose(fid);

    return 0;

}











