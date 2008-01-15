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
    hid_t gid2;         /* group ID */

    hid_t fapl;		/* file access property list */

/* =====  Main processing  ===== */

   /* Open File */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_libver_bounds(fapl, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Open group g3 */
    gid = H5Gopen(fid, "g3", H5P_DEFAULT);

   /* add more groups */
    gid2 = H5Gcreate(gid, "g3.0", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.1", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.2", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.3", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.4", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.5", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.6", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.7", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.8", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

    gid2 = H5Gcreate(gid, "g3.9", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Gclose(gid2);

   /* Close File */
    H5Gclose(gid);
    H5Fclose(fid);
    H5Pclose(fapl);

    return 0;

}











