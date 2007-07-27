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
    hid_t tid;   	/* datatype ID */
    hid_t dcpl;		/* dataset creation property list */

    hid_t fapl;		/* file access property list */

/* =====  Main processing  ===== */

   /* Open File */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_latest_format(fapl, TRUE);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* remove group g4 */
    H5Ldelete( fid, "g4", H5P_DEFAULT );

   /* Create Group g4 */
    gid = H5Gcreate (fid, "/g4", 0);
    H5Lcreate_hard( fid, "/g3/hlink2", gid, "dset2", H5P_DEFAULT, H5P_DEFAULT );
    
   /* Close Group */
    H5Gclose(gid);

   /* Close File */
    H5Fclose(fid);

    return 0;

}











