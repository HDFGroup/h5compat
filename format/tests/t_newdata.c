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

   /* Open Group g3 and remove link to dataset */
    gid = H5Gopen (fid, "/g3");
    H5Ldelete( gid, "hlink2", H5P_DEFAULT );
    H5Gclose(gid);

   /* Open Group g4 */
    gid = H5Gopen (fid, "/g4");
    H5Ldelete( gid, "dset2", H5P_DEFAULT );

   /* Create Dataspace  */
    dims[0] = dims[1] = 10;
    sid = H5Screate_simple(2, dims, NULL);

   /* Create Dataset dset2  */
    did = H5Dcreate(gid, "dset2", H5T_NATIVE_INT, sid, H5P_DEFAULT);

   /* Write to Dataset dset2 */
    for (i = 0; i < 10; i++)
        for (j = 0; j < 10; j++)
            dset2[i][j] = j + 10 * i;
    H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset2);

   /* Close some things */
    H5Sclose(sid);

   /* Add attributes to dset2 */
    for (i = 0; i < 10; i++){
        dims[0] = 10;
        sprintf(buf, "attr%05d", i);
        sid = H5Screate_simple(1, dims, NULL);
        aid = H5Acreate (did, buf, H5T_NATIVE_CHAR, sid, H5P_DEFAULT);
        H5Awrite(aid, H5T_NATIVE_CHAR, buf);
        H5Sclose(sid);
        H5Aclose(aid);
    }
    
   /* Close some things */
    H5Dclose(did);
    H5Gclose(gid);

 /* =========================================== */

   /* Open Group g3 */
    gid = H5Gopen (fid, "/g3");

   /* Create Hard Link to dset2 */
    H5Glink( gid, H5G_LINK_HARD, "/g4/dset2", "./hlink2" );
    H5Gclose(gid);

   /* Close File */
    H5Fclose(fid);

    return 0;

}











