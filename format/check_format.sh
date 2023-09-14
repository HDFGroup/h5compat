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
#  This file:  check_format.sh
# Written by:  Peter Laird
#       Date:  Aug 1, 2007
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                       	#
# This script will:                                                     	#
#	- run compatibility tests using 2 different libraries			#
#           - gen_compat.c is run using the v1.6 library to create compat.h5	#
#	    - a test is run using the v1.8 library to modify compat.h5		#
#	    - read_compat.c is run using the v1.6 library to read compat.h5	#
# Extended May 2019 to also run using v1.10 and vdev (develop branch) libraries #
# in addition to the v1.8 library, and October 2019 for v1.12.                  # 
#                                                                       	#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

HOST_NAME=`hostname | cut -f1 -d.`

# If this script is running in one of the test directories with a suffix such as -64 or -pp it's probably best to run with binaries in a similar directory under pre-release.  An exact match is preferred, but if one with a dash can't be found, try the suffix without the dash.
HOST_DIR=`pwd | awk -F/ '{ print $(NF-1) }'`

STRLEN=`echo "$HOST_DIR" | awk '{ print length() }'`
STRLEN=`expr $STRLEN - 2`

HOST_TEST1=`perl -e "print substr($HOST_DIR, 0, $STRLEN);"`
HOST_TEST2=`echo $HOST_DIR | sed -e 's/-64//' -e 's/-pp//'`

#check for hostname-64 or hostname64 
if [ "${HOST_TEST1}" = "$HOST_NAME" ] || [ "${HOST_TEST2}" = "$HOST_NAME" ];then
   if [ -d /mnt/scr1/pre-release/hdf5/v180/$HOST_DIR ] || [ -d /mnt/scr1/pre-release/hdf5/v18/$HOST_DIR ];then
      HOST_NAME=$HOST_DIR
   else
      SUFF=`echo $HOST_DIR | cut -f2 -d-`
      HOST_NAME=$HOST_NAME$SUFF
   fi
fi

# Define libraries to use
h5ccdev="/mnt/scr1/pre-release/hdf5/vdev/$HOST_NAME/bin/h5cc"
h5cc114="/mnt/scr1/pre-release/hdf5/v114/$HOST_NAME/bin/h5cc"
h5cc112="/mnt/scr1/pre-release/hdf5/v112/$HOST_NAME/bin/h5cc"
h5cc110="/mnt/scr1/pre-release/hdf5/v110/$HOST_NAME/bin/h5cc"
h5cc18="/mnt/scr1/pre-release/hdf5/v18/$HOST_NAME/bin/h5cc"
h5cc16="/mnt/scr1/pre-release/hdf5/v16/$HOST_NAME/bin/h5cc"

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
if [ ! -x $h5cc110 ]; then
    echo "h5cc110($h5cc110) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc112 ]; then
    echo "h5cc112($h5cc112) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc114 ]; then
    echo "h5cc114($h5cc114) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5ccdev ]; then
    echo "h5ccdev($h5ccdev) not found or not executable.  Abort"
    exit 1
fi
# When an error occurs, this file is filled with the error information 
ErrorFile="CompatibilityError.log"

####  Read with v1.6 ####
read16()
{
    $h5cc16 read_compat.c
    if [ $? -eq 0 ]
    then
        echo "========= Reading with v1.6 =========" > errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_compat.c with v1.6"
    fi
}


#### Read with v1.8 ####
read18()
{
    $h5cc18 -DH5_USE_16_API read_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.8 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_compat.c with v1.8"
    fi
}

#### Read with v1.10 ####
read110()
{
    $h5cc110 -DH5_USE_16_API read_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.10 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_compat.c with v1.10"
    fi
}

#### Read with v1.12 ####
read112()
{
    $h5cc112 -DH5_USE_16_API read_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.12 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_compat.c with v1.12"
    fi
}

#### Read with v1.14 ####
read114()
{
    $h5cc114 -DH5_USE_16_API read_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.14 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_compat.c with v1.14"
    fi
}

#### Read with vdev ####
readdev()
{
    $h5ccdev -DH5_USE_16_API read_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with vdev =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_compat.c with vdev"
    fi
}
####  Read with v1.6 ####
read_ref_compat_16()
{
    $h5cc16 read_ref_compat.c
    if [ $? -eq 0 ]
    then
        echo "========= Reading with v1.6 =========" > errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_ref_compat.c with v1.6"
    fi
}


#### Read with v1.8 ####
read_ref_compat_18()
{
    $h5cc18 -DH5_USE_16_API read_ref_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.8 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_ref_compat.c with v1.8"
    fi
}

#### Read with v1.10 ####
read_ref_compat_110()
{
    $h5cc110 -DH5_USE_16_API read_ref_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.10 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_ref_compat.c with v1.10"
    fi
}

#### Read with v1.12 ####
read_ref_compat_112()
{
    $h5cc112 -DH5_USE_16_API read_ref_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.12 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_ref_compat.c with v1.12"
    fi
}

#### Read with v1.14 ####
read_ref_compat_114()
{
    $h5cc114 -DH5_USE_16_API read_ref_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with v1.14 =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_ref_compat.c with v1.14"
    fi
}

#### Read with vdev ####
read_ref_compat_dev()
{
    $h5ccdev -DH5_USE_16_API read_ref_compat.c
    if [ $? -eq 0 ]
    then
        echo >> errors.log
        echo >> errors.log
        echo "========= Reading with vdev =========" >> errors.log
        echo >> errors.log
        ./a.out 2>/dev/null
    else
        echo "messed up compiling read_ref_compat.c with vdev"
    fi
}

#### Check Errors ####
CheckErrors()
{
    expected=$1"-expected"

   # Check if output from reading file is the same as expected output
    cmp -s errors.log tests/expected/$expected
    ret=$?
    if [ $ret -eq 0 ]
     # Output matched expected output
    then
	echo "Test ran as expected"

     # Output file doesn't exist
    elif [ $ret -eq  2 ]
    then
	 # with the -a flag a new expected file will be created
	if [[ $AddExpected == "on" ]]
	then
	    echo "Expected output was not found."
	    echo "Adding current output as expected output:"
	    echo
	    cat errors.log | tee tests/expected/$expected
	    echo
	    echo
	 # without the -a flag, and error will occur
	else
	    echo "!!! Error: There was an error running this test !!!"
            echo " The file containing expected output did not exist"
            echo

            rm a.out
            rm gen_compat.out
            rm *.o
            rm compat.h5
	    rm errors.log
	    exit 1
	fi

     # Output did not match expected output
    else
	echo "!!! Error: There was an error running this test !!!"
	echo "          check $ErrorFile"
	echo


        echo "Test: $1" > $ErrorFile

        echo >> $ErrorFile
        echo "###########################################################" >> $ErrorFile
        echo >> $ErrorFile
        echo "Difference in output:" >> $ErrorFile
	diff errors.log tests/expected/$expected >> $ErrorFile

	echo >> $ErrorFile 
	echo "###########################################################" >> $ErrorFile
	echo >> $ErrorFile
	echo "Expected Output:" >> $ErrorFile
	echo >> $ErrorFile
	cat tests/expected/$expected >> $ErrorFile

	echo >> $ErrorFile
        echo "###########################################################" >> $ErrorFile
        echo >> $ErrorFile
        echo "Output from test:" >> $ErrorFile
        echo >> $ErrorFile
        cat errors.log >> $ErrorFile


        rm a.out
	rm gen_compat.out
	rm *.o
	rm compat.h5
	exit 1
    fi
}

#### Run test ####
RunTest()
{
    Test=$1".c"

    echo
    echo "#################  $1  #################"
    ./gen_compat.out
    $CC tests/$Test
    #$h5cc18 tests/$Test
    if [ $? -ne 0 ]
    then
        echo "messed up compiling test/$Test with $CC"
        exit 1
    fi
    ./a.out
    read16
    read18
    read110
    read112
    read114
    readdev
    CheckErrors $1
    rm errors.log
}


#### Run ref_compat test ####
# Testing 2 different types of references results in different expected output
# depending on the HDF5 version used to create the h5cc script.  Therefore the 
# output is different for a test file created by $h5cc110 or older than for a 
# test file created by $h5cc112 or newer.
Run_ref_compat_Test()
{
    Test=$1".c"

    echo
    echo "#################  $1  #################"
    ./gen_ref_compat.out
    $CC tests/$Test
    if [ $? -ne 0 ]
    then
        echo "messed up compiling test/$Test with $CC"
        exit 1
    fi
    ./a.out
    read_ref_compat_16
    read_ref_compat_18
    read_ref_compat_110
    read_ref_compat_112
    read_ref_compat_114
    read_ref_compat_dev
    if [ "$CC" = "$h5cc18" -o "$CC" = "$h5cc110" ]; then
        CheckErrors $11
    else
        CheckErrors $12
    fi
    rm errors.log
}



##################  MAIN  ##################

if  [ $# -gt 0 ]
then
    if [[ $1 == "-a" ]]
    then
	AddExpected="on"
    fi
fi 

CompVERSIONS="$h5cc18 $h5cc110 $h5cc112 $h5ccdev"
for CC in $CompVERSIONS; do

# Compile gen_compat.c with v1.6
$h5cc16 -o gen_compat.out gen_compat.c
$h5cc16 -o gen_ref_compat.out gen_ref_compat.c
echo "Compiling tests with $CC"

# Run tests
if [ $? -eq 0 ]
then
    if (
        RunTest t_newfile &&\
        RunTest t_newgroup &&\
        RunTest t_newdata &&\
        RunTest t_newlink &&\
        RunTest t_newtype &&\
        RunTest t_newatts &&\
        RunTest t_latest_mod_data &&\
        RunTest t_latest_mod_attr &&\
        RunTest t_latest_more_groups &&\
        RunTest t_index_link &&\
        Run_ref_compat_Test t_ref); then
        EXIT_VALUE=0
    else
        EXIT_VALUE=2
    fi
else
    echo "messed up compiling gen_compat.c"
fi

# Cleanup
rm a.out
rm gen_compat.out
rm gen_ref_compat.out
rm *.o
rm compat.h5
rm ref_compat.h5
echo

done

exit $EXIT_VALUE

