#include "hdf5.h"
#include <string.h>
#include <stdio.h>

#define FILEPATH "./errors.log"
#define FILENAME "compat.h5"

/*================================================
                       /
     |           |        |      |      | 
     g1          g2       g3     g4     g5
    / \          |        |      |      |
 g1.1 g1.2     dtype1   hlink1  dset2  slink2
  |     |     (dset1)   (dset2)        (dset2)
 dset1 slink1
       (dset1)
================================================*/

/* file */

int check_file(hid_t *fid)
{
    if((*fid = H5Fopen(FILENAME, H5F_ACC_RDONLY, H5P_DEFAULT)) <0) return -1;
    return 0;
}

/* group */

int check_group(hid_t *gid, hid_t fid, char group_name[])
{
    if((*gid = H5Gopen(fid, group_name)) <0) return -1;
    return 0;
}

/* datatype */

int check_dtype(hid_t *tid, hid_t gid, char type_name[])
{
    if((*tid = H5Topen(gid, type_name)) <0) return -1;
    return 0;
}

/* child group */

int check_chgroup(hid_t *gid2, hid_t gid, char group_name[])
{
    if((*gid2 = H5Gopen(gid, group_name)) <0) return -1;
    return 0;
}

/* dataset dset1 (for any link to dset1) */

int check_dset1(hid_t *did, hid_t gid2, char link_name[])
{
    if((*did = H5Dopen(gid2, link_name)) <0) return -1;
    return 0;
}

/* data in dset2 */

int check_data1(hid_t did)
{
    typedef struct { /* compound type has members with rank > 1 */
      int a[2][2][2][2]; /* arrays are 2x2x2x2    */
      double b[2][2][2][2];
      double c[2][2][2][2];
    } dset1_t;
    dset1_t buf[6];

    int j, i0, i1, i2, i3;
    int idx[4] = {0,1,2,3};
    const int perm[4] = {0,1,2,3};
    hsize_t sdim;
    sdim = 6;

    hid_t tid;

    if((tid = H5Dget_type(did)) <0) return -1;
    H5Dread(did, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);

    for (j=0; j<(int)sdim; j++) {
      for (i3 = 0; i3 < 2; i3++) {
        idx[perm[3]] = i3;
        for (i2 = 0; i2 < 2; i2++) {
          idx[perm[2]] = i2;
          for (i1 = 0; i1 < 2; i1++) {
            idx[perm[1]] = i1;
            for (i0 = 0; i0 < 2; i0++) {
              idx[perm[0]] = i0;

                if(buf[j].a[idx[3]][idx[2]][idx[1]][idx[0]] != i0+j) return -1;
                if(buf[j].b[idx[3]][idx[2]][idx[1]][idx[0]] != (double)(i0+j)) return -1;
              #ifdef WIN32
                if(buf[j].c[idx[3]][idx[2]][idx[1]][idx[0]] != (double)(i0+j+(signed __int64)sdim)) return -1;
              #else
                if(buf[j].c[idx[3]][idx[2]][idx[1]][idx[0]] != (double)(i0+j+sdim)) return -1;
              #endif
            }
          }
        }
      }
    }

    return 0;
}


/* dataset dset2 (for any link to dset2) */

int check_dset2(hid_t *did, hid_t gid, char link_name[])
{
    if((*did = H5Dopen(gid, link_name)) <0) return -1;

    return 0;
}

/* data in dset2 */

int check_data2(hid_t did)
{
    int buf[10][10];
    int i, j;

    H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);

    for (i = 0; i < 10; i++){
        for (j = 0; j < 10; j++){
            if(buf[i][j] != j + 10 * i) return -1;
        }
    }

    return 0;
}

/* attr (for any attr) */

int check_attr(hid_t did, char attr_name[])
{
    hid_t aid;
    char buf[10];
    if((aid = H5Aopen_name(did, attr_name)) <0) return -1;
    H5Aread(aid, H5T_NATIVE_CHAR, buf );
    H5Aclose(aid);

    if(strcmp (buf, attr_name) != 0) return -1;
    return 0;
}



/* Print outcome to file */

int check(int ret, char type[], char name[15], char message[], FILE *fp)
{
    if (ret < 0){
	fprintf(fp, "Error: %s %s %s\n", type, name, message);
    }
    else{
	fprintf(fp, "Passed: %s %s %s\n", type, name, message);
    }

    return ret;
}

int main(void)
{
    FILE *fp;

    hid_t fid;		/* file ID */
    hid_t gid;          /* group ID */
    hid_t gid2;		/* child group ID */
    hid_t tid;   	/* datatype ID */
    hid_t did;          /* dataset ID */

    char attr_name[15];

    int I;

/* =====  Main processing  ===== */
    fp = fopen(FILEPATH, "w");

    if(check( check_file(&fid), "file", "file", NULL, fp) >=0)
    {
    	/* group g1 */
    	if(check( check_group(&gid, fid, "g1"), "group", "g1", NULL, fp) >=0)
        {
    	    if(check( check_chgroup(&gid2, gid, "g1.1"), "group", "g1.1", NULL, fp) >=0)
	    {
            	if(check( check_dset1(&did, gid2, "dset1"), "dset", "dset1", NULL, fp) >=0)
		{
		    check( check_data1(did), "data", "data1", NULL, fp);
		    H5Dclose(did);
		}
            	H5Gclose(gid2);
    	    }

    	    if(check( check_chgroup(&gid2, gid, "g1.2"), "group", "g1.2", NULL, fp) >=0)
	    {
		if(check( check_dset1(&did, gid2, "hlink1"), "dset", "hlink1", NULL, fp) >0)
		{
		    H5Dclose(did);
		}
            	H5Gclose(gid2);
    	    }

            H5Gclose(gid);
    	}
        /* end group g1 */


	/* group g2 */
        if(check( check_group(&gid, fid, "g2"), "group", "g2", NULL, fp) >=0)
	{
	    /* open dtype1 */
            if(check( check_dtype(&tid, gid, "dtype1"), "dtype", "dtype1", NULL, fp) >=0)
	    {
		/* read dataset using tid */
		H5Tclose(tid);
	    }
	}
        /* end group g2 */


        /* group g3 */
        if(check( check_group(&gid, fid, "g3"), "group", "g3", NULL, fp) >=0)
	{
            /* open hlink1 */
            if(check( check_dset2(&did, gid, "hlink2"), "dset", "hlink2", "(through group g3)", fp) >=0)
	    {
		H5Dclose(did);
	    }

            H5Gclose(gid);
        }
        /* end group g3 */


        /* group g4 */
        if(check( check_group(&gid, fid, "g4"), "group", "g4", NULL, fp) >=0)
        {
            /* open hlink1 */
            if(check( check_dset2(&did, gid, "dset2"), "dset", "dset2", "(through group g4)", fp) >=0)
            {
		check( check_data2(did), "data", "data2", NULL, fp);
		for (I = 0; I < 10; I++)
		{
		    sprintf(attr_name, "attr%05d", I);
		    check( check_attr(did, attr_name), "attr", attr_name, NULL, fp);
		}
                H5Dclose(did);
            }

            H5Gclose(gid);
        }
        /* end group g4 */


        /* group g5 */
        if(check( check_group(&gid, fid, "g5"), "group", "g5", NULL, fp) >=0)
        {
            /* open hlink1 */
            if(check( check_dset2(&did, gid, "slink1"), "dset", "slink1", "(through group g5)", fp) >=0)
            {
                H5Dclose(did);
            }

            H5Gclose(gid);
        }
        /* end group g5 */

    }

    fclose(fp);

    return 0;
}
