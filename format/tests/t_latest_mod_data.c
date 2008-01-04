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
    hid_t aid;          /* attribute ID */
    hid_t did;          /* dataset ID */
    hid_t sid;		/* dataspace ID */

    hid_t fapl;		/* file access property list */

    hsize_t dims[2];

    int dset2[10][10];

    char buf[60];
    int i, j;
    int i0, i1, i2, i3;
    hsize_t sdim, maxdim;

/* =====  Main processing  ===== */

   /* Open File */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_format_bounds(fapl, H5F_FORMAT_LATEST, H5F_FORMAT_LATEST);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Open Group g4 */
    gid = H5Gopen(fid, "/g4", H5P_DEFAULT);

   /* Open Dataspace  */
    did = H5Dopen(gid, "dset2", H5P_DEFAULT);

   /* Write to Dataset dset2 */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            dset2[i][j] = j + 10 * i;
    H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset2);

   /* Close everything */
    H5Dclose(did);
    H5Gclose(gid);
    H5Fclose(fid);
    H5Pclose(fapl);

    return 0;

}











