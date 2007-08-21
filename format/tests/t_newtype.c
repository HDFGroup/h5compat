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
    hid_t did;          /* dataset ID */
    hid_t sid;		/* dataspace ID */
    hid_t tid;   	/* datatype ID */
    hid_t dcpl;		/* dataset creation property list */
    hid_t array_dt;	/* */

    hid_t fapl;		/* file access property list */

    hsize_t dims[2];

    int j;
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

   /* Open File */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_latest_format(fapl, TRUE);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Open Group g2 */
    gid = H5Gopen (fid, "/g2");

   /* Remove link to datatype */
    H5Ldelete(gid, "dtype1", H5P_DEFAULT);

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

   /* Save datatype as a named datatype */
    H5Tcommit(gid, "dtype1", tid );

    H5Tclose(tid);
    H5Gclose(gid);
    H5Fclose(fid);

 /* =========================================== */

   /* Open File */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_latest_format(fapl, FALSE);
    fid = H5Fopen(FILENAME, H5F_ACC_RDWR, fapl);

   /* Open Group g2 */
    gid = H5Gopen (fid, "/g2");

   /* Save datatype as a named datatype */
    tid = H5Topen(gid, "dtype1");

    H5Gclose(gid);

 /* =========================================== */


   /* Open Group g1.1 */
    gid = H5Gopen (fid, "/g1/g1.1");
    H5Ldelete(gid, "dset1", H5P_DEFAULT);

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

   /* Close some things */
    H5Dclose(did);
    H5Pclose(dcpl);
    H5Sclose(sid);
    H5Tclose(tid);
    H5Gclose(gid);

   /* Open Group g1.2 */
    gid = H5Gopen(fid, "/g1/g1.2");
    H5Ldelete(gid, "hlink1", H5P_DEFAULT);

   /* Create Hard Link to dset1 */
    H5Glink( gid, H5G_LINK_HARD, "/g1/g1.1/dset1", "./hlink1" );

    H5Gclose(gid);

   /* Close File */
    H5Fclose(fid);

    return 0;

}











