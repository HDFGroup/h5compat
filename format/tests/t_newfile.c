#include "hdf5.h"

#define FILENAME "compat.h5"


#ifndef TRUE
#define TRUE 1
#endif /* TRUE */
#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

/*================================================
                       /
     |           |        |      |      | 
     g1          g2       g3     g4     g5
    / \          |        |      |      |
 g1.1 g1.2     dtype1   hlink2  dset2  slink1
  |     |     (dset1)   (dset2)        (dset2)
 dset1 hlink1
       (dset1)
================================================*/

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
    hid_t array_dt;	/* */

    hid_t fapl;		/* file access property list */

    hsize_t dims[2];

    int data[2][2], dset2[10][10];
    double d[10];

    char buf[60];
    int i, j;
    int i0, i1, i2, i3;
    hsize_t sdim, maxdim;

    typedef struct { /* compound type has members with rank > 1 */
      int a[2][2][2][2]; /* arrays are 2x2x2x2    */
      double b[2][2][2][2];
      double c[2][2][2][2];
    } dset1_t;
    dset1_t dset1[6];

    hsize_t dim[4];
    int idx[4] = {0,1,2,3};  /* normal indicies */
    const int perm[4] = {0,1,2,3};  /* the 0'th and the 3'rd indices are permuted */

/* =====  Main processing  ===== */

   /* Create File */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_latest_format(fapl, TRUE);
    fid = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    H5Fclose(fid);

    H5Pset_latest_format(fapl, FALSE);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Create Group g1 */
    gid = H5Gcreate (fid, "/g1", 0);
    H5Gclose(gid);

   /* Create Group g1.1 */
    gid = H5Gcreate (fid, "/g1/g1.1", 0);

   /* Create Datatype tid */
    dim[0] = dim[1] = dim[2] = dim[3] = 2;
    tid = H5Tcreate(H5T_COMPOUND, sizeof(dset1_t));

    array_dt = H5Tarray_create(H5T_NATIVE_INT, 4, dim, NULL);
    H5Tinsert(tid, "a_array", HOFFSET(dset1_t, a), array_dt);
    H5Tclose(array_dt);

    array_dt = H5Tarray_create(H5T_NATIVE_DOUBLE, 4, dim, NULL);
    H5Tinsert(tid, "b_array", HOFFSET(dset1_t, b), array_dt);
    H5Tclose(array_dt);

    array_dt = H5Tarray_create(H5T_NATIVE_DOUBLE, 4, dim, NULL);
    H5Tinsert(tid, "c_array", HOFFSET(dset1_t, c), array_dt);
    H5Tclose(array_dt);

   /* Create Dataspace */
    sdim = 6;
    maxdim = H5S_UNLIMITED;
    sid = H5Screate_simple(1, &sdim, &maxdim);

   /* Create Dataset dset1 */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_chunk(dcpl, 1, &sdim);
    did = H5Dcreate(gid, "dset1", tid, sid, dcpl);

   /* Write to Dataset dset1 */
    for (j=0; j<(int)sdim; j++) {
      for (i3 = 0; i3 < 2; i3++) {
        idx[perm[3]] = i3;
        for (i2 = 0; i2 < 2; i2++) {
          idx[perm[2]] = i2;
          for (i1 = 0; i1 < 2; i1++) {
            idx[perm[1]] = i1;
            for (i0 = 0; i0 < 2; i0++) {
              idx[perm[0]] = i0;

		dset1[j].a[idx[3]][idx[2]][idx[1]][idx[0]] = i0+j;
		dset1[j].b[idx[3]][idx[2]][idx[1]][idx[0]] = (double)(i0+j);
	      #ifdef WIN32
		dset1[j].c[idx[3]][idx[2]][idx[1]][idx[0]] = (double)(i0+j+(signed __int64)sdim);
	      #else
		dset1[j].c[idx[3]][idx[2]][idx[1]][idx[0]] = (double)(i0+j+sdim);
	      #endif
	    }
	  }
	}
      }
    }

    H5Dwrite(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset1);

   /* Save datatype as a named datatype */

   /* Close some things */
    H5Dclose(did);
    H5Pclose(dcpl);
    H5Sclose(sid);
    H5Gclose(gid);

   /* Create Group g1.2 */
    gid = H5Gcreate (fid, "/g1/g1.2", 0);

   /* Create Soft Link to dset1 */
    H5Glink( gid, H5G_LINK_HARD, "/g1/g1.1/dset1", "./hlink1" );

   /* Save datatype as a named datatype */

    H5Gclose(gid);

 /* =========================================== */

   /* Create Group g2 */
    gid = H5Gcreate (fid, "/g2", 0);

   /* Save datatype as a named datatype */
    H5Tcommit(gid, "dtype1", tid );
    H5Tclose(tid);
    H5Gclose(gid);

 /* =========================================== */

   /* Create Group g3 */
    gid = H5Gcreate (fid, "/g3", 0);
    H5Gclose(gid);

 /* =========================================== */

   /* Create Group g4 */
    gid = H5Gcreate (fid, "/g4", 0);

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

 /* =========================================== */

   /* Create Group g5 */
    gid = H5Gcreate (fid, "/g5", 0);

   /* Create Hard Link to dset2 */
    H5Glink( gid, H5G_LINK_SOFT, "/g4/dset2", "./slink1" );

    H5Gclose(gid);


   /* Close File */
    H5Fclose(fid);

    return 0;

}











