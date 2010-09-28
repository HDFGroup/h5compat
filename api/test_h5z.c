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

/*
 * Basic tests of dataset (H5D) API routines, to verify that API compatibility
 *      is working in the 1.8+ versions of the library
 */

#define FILENAME        "compat_h5z.h5"
#define DSET_NAME       "/Dataset"

/* Empty filter function */
static size_t
filter_func(unsigned int flags, size_t cd_nelmts,
        const unsigned int cd_values[], size_t nbytes, size_t *buf_size,
        void **buf)
{
    /* Prevent compiler warning */
    flags = flags;
    cd_nelmts = cd_nelmts;
    cd_values = cd_values;
    buf_size = buf_size;
    buf = buf;

    return(nbytes);
} /* end filter_func */

int
main(int argc, const char *argv[])
{
    hid_t       dcpl;           /* Dataset Creation Property List */
    H5Z_class_t filter_class;   /* Filter class */

    /* Shut compiler up */
    argc = argc;
    argv = argv;

    /* Dump versions for API symbols tested, if library supports versioning */
#if H5_VERS_MINOR >= 8
    printf("H5Z_class_t_vers = %d\n", H5Z_class_t_vers);
#endif /* H5_VERS_MINOR >= 8 */

    /* Create file */
    if((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0) goto error;

    /* Create filter */
    filter_class.id = 365;
    filter_class.name = "compat_filter";
    filter_class.can_apply = NULL;
    filter_class.set_local = NULL;
    filter_class.filter = filter_func;
#if defined(H5Z_class_t_vers) && H5Z_class_t_vers > 1
    filter_class.version = H5Z_CLASS_T_VERS;
    filter_class.encoder_present = 1;
    filter_class.decoder_present = 1;
#endif /* H5Z_class_t_vers */

    /* Register the filter */
    if(H5Zregister(&filter_class) < 0) goto error;

    /* Add the filter to the property list */
    if(H5Pset_filter(dcpl, 365, 0, (size_t)0, NULL) < 0) goto error;

    /* Close */
    if(H5Pclose(dcpl) < 0) goto error;


    return(0);

error:
    return(1);
}

