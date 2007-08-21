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
    H5Pset_latest_format(fapl, TRUE);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Open Group g4 */
    gid = H5Gopen(fid, "/g4");

   /* Open Dataspace  */
    did = H5Dopen(gid, "dset2");

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











