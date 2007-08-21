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











