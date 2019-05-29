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

#if defined(H5F_LIBVER_18)
#define H5F_LIBVER_V18 H5F_LIBVER_18
#endif

#ifndef TRUE
#define TRUE 1
#endif /* TRUE */
#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

int main(int argc, char *argv[])
{
/* =====  Variables  ===== */

    hid_t fid;		/* file ID */
    hid_t gid;          /* group ID */
    hid_t aid;          /* attribute ID */
    hid_t did;          /* dataset ID */
    hid_t sid;		/* dataspace ID */
    hid_t tid;   	/* datatype ID */
    hid_t dcpl;		/* dataset creation property list */

    hid_t fapl;		/* file access property list */

/* =====  Main processing  ===== */

   /* Open File */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    if(H5F_LIBVER_LATEST > 1)
        H5Pset_libver_bounds(fapl, H5F_LIBVER_V18, H5F_LIBVER_LATEST);
    else
        H5Pset_libver_bounds(fapl, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Open group g3 */
    gid = H5Gopen(fid, "/g3", H5P_DEFAULT);
    H5Ldelete( gid, "hlink2", H5P_DEFAULT );

   /* Create Link hlink2 */
    H5Lcreate_hard( fid, "/g4/dset2", gid, "hlink2", H5P_DEFAULT, H5P_DEFAULT );
    
   /* Close Group */
    H5Gclose(gid);

   /* Close File */
    H5Fclose(fid);

    return 0;

}











