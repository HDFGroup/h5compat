#! /bin/sh
#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the files COPYING and Copyright.html.  COPYING can be found at the root
# of the source code distribution tree; Copyright.html can be found at the
# root level of an installed copy of the electronic HDF5 document set and
# is linked from the top-level documents page.  It can also be found at
# http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have
# access to either file, you may request a copy from help@hdfgroup.org.

#
#  This file:  check_api.sh
# Written by:  Quincey Koziol
#       Date:  Aug 21, 2007
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                       	#
# This script will:                                                     	#
#	- run compatibility tests using 2 different libraries:			#
#           - Each test is designed to be compiled with the 1.6.x library API	#
#	    - The h5cc script is used from a "normal" 1.6.x build		#
#	    - The h5cc script from a 1.8.x distribution built with the		#
#		 --with-default-api-version=v16 flag				#
#                                                                       	#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


# Define compile scripts to use
h5cc18="/mnt/scr1/pre-release/hdf5/v180/kagiso/bin/h5cc"
h5cc16="/mnt/scr1/pre-release/hdf5/v16/kagiso/bin/h5cc"

# Definitions
initfname=.h5compatrc		# personal initialization file

# Look for the personal initialization file in $PWD, then in $HOME.
# If none found, keep preset values.
if [ -r ./$initfname ]; then
    echo scan in ./$initfname
    . ./$initfname
elif [ -r $HOME/$initfname ]; then
    echo scan in $HOME/$initfname
    . $HOME/$initfname
else
    echo No personal initialization file found.  Keep preset values.
fi

# Check definitions
if [ ! -x $h5cc16 ]; then
    echo "h5cc16($h5cc16) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc18 ]; then
    echo "h5cc18($h5cc18) not found or not executable.  Abort"
    exit 1
fi


# Build test with compiler script and run test
build()
{
    $1 $2.c
    if (($? == 0))
    then
        echo "========= Testing with $1 ========="
        ./a.out 2>/dev/null
        if (($? != 0))
        then
            echo "error running $2 with $1"
            exit 1
        fi
    else
        echo "error compiling $2.c with $1"
        exit 1
    fi
}

#### Run test ####
RunTest()
{
    echo
    echo "################# Testing $1 #################"
    build $h5cc16 $1
    build $h5cc18 $1
}



##################  MAIN  ##################

# Run tests
RunTest test_h5e

# Cleanup
rm a.out
rm *.o
rm *.h5
echo

exit 0

