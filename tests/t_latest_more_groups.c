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
    H5Pset_latest_format(fapl, TRUE);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Open group g3 */
    gid = H5Gopen( fid, "g3");

   /* add more groups */
    gid2 = H5Gcreate( gid, "g3.0", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.1", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.2", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.3", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.4", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.5", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.6", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.7", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.8", 0);
    H5Gclose(gid2);

    gid2 = H5Gcreate( gid, "g3.9", 0);
    H5Gclose(gid2);

   /* Close File */
    H5Gclose(gid);
    H5Fclose(fid);
    H5Pclose(fapl);

    return 0;

}











