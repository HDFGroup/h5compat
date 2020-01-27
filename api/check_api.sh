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
#                                                                               #
# Extended May 2019 to also use h5cc scripts from 1.10.x built with the         #
#                --with-default-api-version=v16 flag                            #    
#                --with-default-api-version=v18 flag                            #
#             flags.                                                            #
#                                                                               # 
# Extended October 2019 to also use h5cc scripts from 1.12.x.built with the     #
#                --with-default-api-version=v16                                 #
#                --with-default-api-version=v18                                 #
#                --with-default-api-version=v110                                #
#             and h5cc scripts from the develop branch (1.13/1.14) with         #
#                --with-default-api-version=v16                                 #
#                --with-default-api-version=v18                                 #
#                --with-default-api-version=v110                                #
#                --with-default-api-version=v112                                #
#             flags.                                                            #
#                                                                               # 
#             Tests also run with the "normal" builds and the                   #
#                -DH5_USE_16_API                                                #
#                -DH5_USE_18_API                                                #
#                -DH5_USE_110_API                                               #
#                -DH5_USE_112_API                                               #
#             macros.                                                           #  
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


##################################
# Script Functions for later use #
##################################


# Print a line-line message left justified in a field of 70 characters
# beginning with the word "Testing".
#
TESTING() {
   SPACES="                                                               "
   echo "Testing $* $SPACES" | cut -c1-70 | tr -d '\012'
}

# Clean up any files produced
#
CLEANUP()
{
    rm *.out
    rm *.o
    rm -f *.h5
}

# Check output from running test program
#
CHECK()
{
    # Set the same of the actual and expected output files
    expected="expected/$1/$2"
    actual=$1".out"
    tmp="tmp.out"

    # Mask off version extensions and thread ID
    sed -e 's/1.6.[0-9].*th/1.6.x th/' -e 's/(1.8.[0-9].*)/(1.8.x)/' -e 's/(1.10.[0-9].*)/(1.10.x)/' -e 's/(1.12.[0-9].*)/(1.12.x)/' -e 's/(1.13.[0-9].*)/(1.13.x)/' -e 's/thread .*:/thread 0:/' <$actual >$tmp
    ret=$?
    if [ $ret -ne 0 ]; then
        echo "sed failed ?!?!"
        exit 1
    fi

    # Check if output from test program is the same as expected output
    cmp -s $tmp $expected
    ret=$?

    # Check if output didn't match expected output
    if [ $ret -ne 0 ]; then
	
        # Output file doesn't exist
        if [ $ret -eq  2 ]; then
            # with the -a flag a new expected file will be created
            if [[ $AddExpected == "on" ]]; then
                echo "Expected output was not found."
                echo "Adding current output as expected output:"
                echo
                cat $tmp | tee $expected
                echo
                echo
            # without the -a flag, and error will occur
            else
                echo "*FAILED*"
                echo "!!! Error: There was an error running this test !!!"
                echo " The file containing expected output did not exist"
                echo

                CLEANUP
                exit 1
            fi

         # Output did not match expected output
        else
            echo "*FAILED*"
            echo "!!! Error: There was an error running this test !!!"
            echo

            echo
            echo "###########################################################"
            echo
            echo "Difference in output:"
            diff $tmp $expected

            echo
            echo "###########################################################"
            echo
            echo "Expected Output:"
            echo
            cat $expected

            echo
            echo "###########################################################"
            echo
            echo "Output from test:"
            echo
            cat $tmp

            CLEANUP
            exit 1
        fi
    fi
}

# Build & run test with compiler script
#
BUILD()
{
    $1 $2.c
    if [ $? -eq 0 ]; then
#        echo "Testing with: $1"
#        ./a.out
        ./a.out > $2.out
        if [ $? -ne 0 ]; then
            echo "*FAILED*"
            echo "error running $2 with $1"
            exit 1
        else
            CHECK $2 $3
        fi
    else
        echo "*FAILED*"
        echo "error compiling $2.c with $1"
        exit 1
    fi
    echo " PASSED"
}

# Build test program with 1.6.x and 1.8.x libraries
#
TEST()
{
    # Create aliases for the parameters
    testname=$1
    compile_options=$2
 
    TESTING "actual 1.6.x library"
    BUILD "$h5cc16 $compile_options" $testname "v16"

    TESTING "normal 1.8.x library build"
    BUILD "$h5cc18 $compile_options" $testname "v18-actual"

    TESTING "normal 1.10.x library build"
    BUILD "$h5cc110 $compile_options" $testname "v110-actual"

    TESTING "normal 1.12.x library build"
    BUILD "$h5cc112 $compile_options" $testname "v112-actual"

    TESTING "normal 1.13.x library build"
    BUILD "$h5ccdev $compile_options" $testname "vdev-actual"

    TESTING "1.8.x library built in 1.6.x compatibility mode"
    BUILD "$h5cc18compat $compile_options" $testname "v18-compat"

    TESTING "normal 1.8.x library build, with 1.6.x compatibility macro"
    BUILD "$h5cc18 -DH5_USE_16_API $compile_options" $testname "v18-macro"

    TESTING "1.10.x library built in 1.6.x compatibility mode"
    BUILD "$h5cc110compat16 $compile_options" $testname "v110-16compat"

    TESTING "normal 1.10.x library build, with 1.6.x compatibility macro"
    BUILD "$h5cc110 -DH5_USE_16_API $compile_options" $testname "v110-16macro"

    TESTING "1.10.x library built in 1.8.x compatibility mode"
    BUILD "$h5cc110compat18 $compile_options" $testname "v110-18compat"

    TESTING "normal 1.10.x library build, with 1.8.x compatibility macro"
    BUILD "$h5cc110 -DH5_USE_18_API $compile_options" $testname "v110-18macro"

    TESTING "1.12.x library built in 1.6.x compatibility mode"
    BUILD "$h5cc112compat16 $compile_options" $testname "v112-16compat"

    TESTING "normal 1.12.x library build, with 1.6.x compatibility macro"
    BUILD "$h5cc112 -DH5_USE_16_API $compile_options" $testname "v112-16macro"

    TESTING "1.12.x library built in 1.8.x compatibility mode"
    BUILD "$h5cc112compat18 $compile_options" $testname "v112-18compat"

    TESTING "normal 1.12.x library build, with 1.8.x compatibility macro"
    BUILD "$h5cc112 -DH5_USE_18_API $compile_options" $testname "v112-18macro"

    TESTING "1.12.x library built in 1.10.x compatibility mode"
    BUILD "$h5cc112compat110 $compile_options" $testname "v112-110compat"

    TESTING "normal 1.12.x library build, with 1.10.x compatibility macro"
    BUILD "$h5cc112 -DH5_USE_110_API $compile_options" $testname "v112-110macro"

    TESTING "1.13.x library built in 1.6.x compatibility mode"
    BUILD "$h5ccdevcompat16 $compile_options" $testname "vdev-16compat"

    TESTING "normal 1.13.x library build, with 1.6.x compatibility macro"
    BUILD "$h5ccdev -DH5_USE_16_API $compile_options" $testname "vdev-16macro"

    TESTING "1.13.x library built in 1.8.x compatibility mode"
    BUILD "$h5ccdevcompat18 $compile_options" $testname "vdev-18compat"

    TESTING "normal 1.13.x library build, with 1.8.x compatibility macro"
    BUILD "$h5ccdev -DH5_USE_18_API $compile_options" $testname "vdev-18macro"

    TESTING "1.13.x library built in 1.10.x compatibility mode"
    BUILD "$h5ccdevcompat110 $compile_options" $testname "vdev-110compat"

    TESTING "normal 1.13.x library build, with 1.10.x compatibility macro"
    BUILD "$h5ccdev -DH5_USE_110_API $compile_options" $testname "vdev-110macro"

    TESTING "1.13.x library built in 1.12.x compatibility mode"
    BUILD "$h5ccdevcompat112 $compile_options" $testname "vdev-112compat"

    TESTING "normal 1.13.x library build, with 1.12.x compatibility macro"
    BUILD "$h5ccdev -DH5_USE_112_API $compile_options" $testname "vdev-112macro"
}

# Build test program with different API routine versions overridden
#
TESTAPI()
{
    # Create aliases for the parameters
    testname=$1
    compile_options=$2
    suffix=$3
    compat_options=$4

    TESTING "normal 1.8.x, with: $compat_options"
    BUILD "$h5cc18 $compat_options $compile_options" $testname "v18-$suffix"
}

# Build test program with different API routine versions overridden
#
TESTAPI110()
{
    # Create aliases for the parameters
    testname=$1
    compile_options=$2
    suffix=$3
    compat_options=$4

    TESTING "normal 1.10.x, with: $compat_options"
    BUILD "$h5cc110 $compat_options $compile_options" $testname "v110-$suffix"
}

# Build test program with different API routine versions overridden
#
TESTAPI112()
{
    # Create aliases for the parameters
    testname=$1
    compile_options=$2
    suffix=$3
    compat_options=$4

    TESTING "normal 1.12.x, with: $compat_options"
    BUILD "$h5cc112 $compat_options $compile_options" $testname "v112-$suffix"
}

# Build test program with different API routine versions overridden
#
TESTAPI114()
{
    # Create aliases for the parameters
    testname=$1
    compile_options=$2
    suffix=$3
    compat_options=$4

    TESTING "normal 1.14.x, with: $compat_options"
    BUILD "$h5ccdev $compat_options $compile_options" $testname "vdev-$suffix"
}

# Runs tests for H5A API
#
TEST_H5A()
{
    compile_options="-DH5Dcreate_vers=1"

    echo
    echo "################# Testing H5A API #################"

    # Run "entire library API" tests
    TEST test_h5a $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5a "$compile_options" H5Acreate1 "-DH5Acreate_vers=1"
    TESTAPI test_h5a "$compile_options" H5Acreate2 "-DH5_USE_16_API -DH5Acreate_vers=2"
    TESTAPI test_h5a "$compile_options" H5Aiterate1 "-DH5Aiterate_vers=1"
    TESTAPI test_h5a "$compile_options" H5Aiterate2 "-DH5_USE_16_API -DH5Aiterate_vers=2"
}

# Runs tests for H5D API
#
TEST_H5D()
{
    compile_options=""

    echo
    echo "################# Testing H5D API #################"

    # Run "entire library API" tests
    TEST test_h5d $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5d "$compile_options" H5Dopen1 "-DH5Dopen_vers=1"
    TESTAPI test_h5d "$compile_options" H5Dopen2 "-DH5_USE_16_API -DH5Dopen_vers=2"
    TESTAPI test_h5d "$compile_options" H5Dcreate1 "-DH5Dcreate_vers=1"
    TESTAPI test_h5d "$compile_options" H5Dcreate2 "-DH5_USE_16_API -DH5Dcreate_vers=2"
}

# Runs tests for H5E API
#
TEST_H5E()
{
    compile_options=""

    echo
    echo "################# Testing H5E API #################"

    # Run "entire library API" tests
    TEST test_h5e $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5e "$compile_options" H5Eclear1 "-DH5Eclear_vers=1"
    TESTAPI test_h5e "$compile_options" H5Eclear2 "-DH5_USE_16_API -DH5Eclear_vers=2"
    TESTAPI test_h5e "$compile_options" H5Eauto1 "-DH5E_auto_t_vers=1 -DH5Eget_auto_vers=1 -DH5Eset_auto_vers=1"
    TESTAPI test_h5e "$compile_options" H5Eauto2 "-DH5_USE_16_API -DH5E_auto_t_vers=2 -DH5Eget_auto_vers=2 -DH5Eset_auto_vers=2"
    TESTAPI test_h5e "$compile_options" H5Epush1 "-DH5Epush_vers=1"
    TESTAPI test_h5e "$compile_options" H5Epush2 "-DH5_USE_16_API -DH5Epush_vers=2"
    TESTAPI test_h5e "$compile_options" H5Ewalk1 "-DH5Ewalk_vers=1"
    TESTAPI test_h5e "$compile_options" H5Ewalk2 "-DH5_USE_16_API -DH5Ewalk_vers=2"
    TESTAPI test_h5e "$compile_options" H5Eprint1 "-DH5Eprint_vers=1"
    TESTAPI test_h5e "$compile_options" H5Eprint2 "-DH5_USE_16_API -DH5Eprint_vers=2"
}

# Runs tests for H5F API
#
TEST_H5F()
{
    compile_options=""

    echo
    echo "################# Testing H5F API #################"

    # Run "entire library API" tests
    TEST test_h5f $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI110 test_h5f "$compile_options" H5Fget_info1 "-DH5Fget_info_vers=1"
    TESTAPI110 test_h5f "$compile_options" H5Fget_info2 "-DH5_USE_18_API -DH5Fget_open_vers=2"
}

# Runs tests for H5G API
#
TEST_H5G()
{
    compile_options=""

    echo
    echo "################# Testing H5G API #################"

    # Run "entire library API" tests
    TEST test_h5g $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5g "$compile_options" H5Gcreate1 "-DH5Gcreate_vers=1"
    TESTAPI test_h5g "$compile_options" H5Gcreate2 "-DH5_USE_16_API -DH5Gcreate_vers=2"
    TESTAPI test_h5g "$compile_options" H5Gopen1 "-DH5Gopen_vers=1"
    TESTAPI test_h5g "$compile_options" H5Gopen2 "-DH5_USE_16_API -DH5Gopen_vers=2"
}

# Runs tests for H5O API
#
TEST_H5O()
{
    compile_options="-DH5Dcreate_vers=1"

    echo
    echo "################# Testing H5O API #################"

    # Run "entire library API" tests
    TEST test_h5o $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI112 test_h5o "$compile_options" H5Oget_info1 "-DH5Oget_info_vers=1 -DH5O_info_t_vers=1 -DH5O_iterate_t_vers=1"
    TESTAPI112 test_h5o "$compile_options" H5Oget_info3 "-DH5_USE_110_API -DH5Oget_info_vers=3 -DH5O_iterate_t_vers=2"
    TESTAPI112 test_h5o "$compile_options" H5Oget_info_by_name1 "-DH5Oget_info_by_name_vers=1 -DH5O_info_t_vers=1 -DH5O_iterate_t_vers=1"
    TESTAPI112 test_h5o "$compile_options" H5Oget_info_by_name3 "-DH5_USE_110_API -DH5Oget_info_by_name_vers=3 -DH5O_iterate_t_vers=2"
    TESTAPI112 test_h5o "$compile_options" H5Oget_info_by_idx1 "-DH5Oget_info_by_idx_vers=1 -DH5O_info_t_vers=1 -DH5O_iterate_t_vers=1"
    TESTAPI112 test_h5o "$compile_options" H5Oget_info_by_idx3 "-DH5_USE_110_API -DH5Oget_info_by_idx_vers=3 -DH5O_iterate_t_vers=2"
    TESTAPI112 test_h5o "$compile_options" H5Ovisit1 "-DH5Ovisit_vers=1 -DH5O_info_t_vers=1"
    TESTAPI112 test_h5o "$compile_options" H5Ovisit3 "-DH5_USE_110_API -DH5Ovisit_vers=3 -DH5O_iterate_t_vers=2"
    TESTAPI112 test_h5o "$compile_options" H5Ovisit_by_name1 "-DH5Ovisit_by_name_vers=1 -DH5O_info_t_vers=1"
    TESTAPI112 test_h5o "$compile_options" H5Ovisit_by_name3 "-DH5_USE_110_API -DH5Ovisit_by_name_vers=3 -DH5O_iterate_t_vers=2"
}

# Runs tests for H5P API
#
TEST_H5P()
{
    compile_options=""

    echo
    echo "################# Testing H5P API #################"

    # Run "entire library API" tests
    TEST test_h5p $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5p "$compile_options" H5Pregister1 "-DH5Pregister_vers=1"
    TESTAPI test_h5p "$compile_options" H5Pregister2 "-DH5_USE_16_API -DH5Pregister_vers=2"
    TESTAPI test_h5p "$compile_options" H5Pinsert1 "-DH5Pinsert_vers=1"
    TESTAPI test_h5p "$compile_options" H5Pinsert2 "-DH5_USE_16_API -DH5Pinsert_vers=2"
    TESTAPI test_h5p "$compile_options" H5Pget_filter1 "-DH5Pget_filter_vers=1"
    TESTAPI test_h5p "$compile_options" H5Pget_filter2 "-DH5_USE_16_API -DH5Pget_filter_vers=2"
    TESTAPI test_h5p "$compile_options" H5Pget_filter_by_id1 "-DH5Pget_filter_by_id_vers=1"
    TESTAPI test_h5p "$compile_options" H5Pget_filter_by_id2 "-DH5_USE_16_API -DH5Pget_filter_by_id_vers=2"

    TESTAPI112 test_h5p "$compile_options" H5Pencode1 "-DH5Pencode_vers=1"
    TESTAPI112 test_h5p "$compile_options" H5Pencode2 "-DH5_USE_110_API -DH5Pencode_vers=2"
}

# Runs tests for H5R API
#
TEST_H5R()
{
    compile_options="-DH5Dcreate_vers=1"

    echo
    echo "################# Testing H5R API #################"

    # Run "entire library API" tests
    TEST test_h5r $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5r "$compile_options" H5Rget_obj_type1 "-DH5Rget_obj_type_vers=1"
    TESTAPI test_h5r "$compile_options" H5Rget_obj_type2 "-DH5_USE_16_API -DH5Rget_obj_type_vers=2"
    TESTAPI112 test_h5r "$compile_options" H5Rdereference1 "-DH5Rdereference_vers=1"
    TESTAPI112 test_h5r "$compile_options" H5Rdereference2 "-DH5_USE_110_API -DH5Rdereference_vers=2"
}

# Runs tests for H5S API
#
TEST_H5S()
{
    compile_options=""

    echo
    echo "################# Testing H5S API #################"

    # Run "entire library API" tests
    TEST test_h5s $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI112 test_h5s "$compile_options" H5Sencode1 "-DH5S_vers=1"
    TESTAPI112 test_h5s "$compile_options" H5Sencode2 "-DH5_USE_110_API -DH5Sencode_vers=2"
}

# Runs tests for H5T API
#
TEST_H5T()
{
    compile_options=""

    echo
    echo "################# Testing H5T API #################"

    # Run "entire library API" tests
    TEST test_h5t $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5t "$compile_options" H5Tarray_create1 "-DH5Tarray_create_vers=1"
    TESTAPI test_h5t "$compile_options" H5Tarray_create2 "-DH5_USE_16_API -DH5Tarray_create_vers=2"
    TESTAPI test_h5t "$compile_options" H5Tcommit1 "-DH5Tcommit_vers=1"
    TESTAPI test_h5t "$compile_options" H5Tcommit2 "-DH5_USE_16_API -DH5Tcommit_vers=2"
    TESTAPI test_h5t "$compile_options" H5Tget_array_dims1 "-DH5Tget_array_dims_vers=1"
    TESTAPI test_h5t "$compile_options" H5Tget_array_dims2 "-DH5_USE_16_API -DH5Tget_array_dims_vers=2"
    TESTAPI test_h5t "$compile_options" H5Topen1 "-DH5Topen_vers=1"
    TESTAPI test_h5t "$compile_options" H5Topen2 "-DH5_USE_16_API -DH5Topen_vers=2"
}

# Runs tests for H5Z API
#
TEST_H5Z()
{
    compile_options=""

    echo
    echo "################# Testing H5Z API #################"

    # Run "entire library API" tests
    TEST test_h5z $compile_options

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5z "$compile_options" H5Z_class1_t "-DH5Z_class_t_vers=1"
    TESTAPI test_h5z "$compile_options" H5Z_class2_t "-DH5_USE_16_API -DH5Z_class_t_vers=2"
}


##################  INITIALIZE SCRIPT INFO  ##################

HOST_NAME=`hostname | cut -f1 -d.`

# If this script is running in one of the test directories with a suffix such as -64 or -pp it's probably best to run with binaries in a similar directory under pre-release.  An exact match is preferred, but if one with a dash can't be found, try the suffix without the dash.
HOST_DIR=`pwd | awk -F/ '{ print $(NF-1) }'`

STRLEN=`echo "$HOST_DIR" | awk '{ print length() }'`
STRLEN=`expr $STRLEN - 2`

HOST_TEST1=`perl -e "print substr($HOST_DIR, 0, $STRLEN);"`
HOST_TEST2=`echo $HOST_DIR | sed -e 's/-64//' -e 's/-pp//'`

if [ "${HOST_TEST1}" = "$HOST_NAME" ] || [ "${HOST_TEST2}" = "$HOST_NAME" ];then
   if [ -d /mnt/scr1/pre-release/hdf5/v180/$HOST_DIR ] || [ -d /mnt/scr1/pre-release/hdf5/v18/$HOST_DIR ];then
      HOST_NAME=$HOST_DIR
   else
      SUFF=`echo $HOST_DIR | cut -f2 -d-`
      HOST_NAME=$HOST_NAME$SUFF
   fi    
fi

# Define compile scripts to use
h5ccdev="/mnt/scr1/pre-release/hdf5/vdev/$HOST_NAME/bin/h5cc"
h5ccdevcompat16="/mnt/scr1/pre-release/hdf5/vdev/compat16/$HOST_NAME/bin/h5cc"
h5ccdevcompat18="/mnt/scr1/pre-release/hdf5/vdev/compat18/$HOST_NAME/bin/h5cc"
h5ccdevcompat110="/mnt/scr1/pre-release/hdf5/vdev/compat110/$HOST_NAME/bin/h5cc"
h5ccdevcompat112="/mnt/scr1/pre-release/hdf5/vdev/compat112/$HOST_NAME/bin/h5cc"
h5cc112="/mnt/scr1/pre-release/hdf5/v112/$HOST_NAME/bin/h5cc"
h5cc112compat16="/mnt/scr1/pre-release/hdf5/v112/compat16/$HOST_NAME/bin/h5cc"
h5cc112compat18="/mnt/scr1/pre-release/hdf5/v112/compat18/$HOST_NAME/bin/h5cc"
h5cc112compat110="/mnt/scr1/pre-release/hdf5/v112/compat110/$HOST_NAME/bin/h5cc"
h5cc110="/mnt/scr1/pre-release/hdf5/v110/$HOST_NAME/bin/h5cc"
h5cc110compat16="/mnt/scr1/pre-release/hdf5/v110/compat16/$HOST_NAME/bin/h5cc"
h5cc110compat18="/mnt/scr1/pre-release/hdf5/v110/compat18/$HOST_NAME/bin/h5cc"
h5cc18="/mnt/scr1/pre-release/hdf5/v18/$HOST_NAME/bin/h5cc"
h5cc18compat="/mnt/scr1/pre-release/hdf5/v18/compat/$HOST_NAME/bin/h5cc"
h5cc16="/mnt/scr1/pre-release/hdf5/v16/$HOST_NAME/bin/h5cc"


# Parse command line arguments
if (( $# > 0 )); then
    if [[ $1 == "-a" ]]; then
	AddExpected="on"
    fi
fi 

# Definitions
initfname=.h5compatrc		# personal initialization file

# Look for the personal initialization file in $PWD, then in $HOME.
# If none found, keep preset values.
echo
echo "Looking for config file ($initfname):"
if [ -r ./$initfname ]; then
    echo Found ./$initfname
    . ./$initfname
elif [ -r $HOME/$initfname ]; then
    echo Found $HOME/$initfname
    . $HOME/$initfname
else
    echo No personal initialization file found.  Keep preset values.
fi

# Check definitions
if [ ! -x $h5ccdev ]; then
    echo "h5ccdev($h5ccdev) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5ccdevcompat112 ]; then
    echo "h5ccdevcompat112($h5ccdevcompat112) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5ccdevcompat110 ]; then
    echo "h5ccdevcompat110($h5ccdevcompat110) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5ccdevcompat18 ]; then
    echo "h5ccdevcompat18($h5ccdevcompat18) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5ccdevcompat16 ]; then
    echo "h5ccdevcompat16($h5ccdevcompat16) not found or not executable.  Abort"
    exit 1
fi

if [ ! -x $h5cc112 ]; then
    echo "h5cc112($h5cc112) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5ccdevcompat112 ]; then
    echo "h5ccdevcompat112($h5ccdevcompat112) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc112compat18 ]; then
    echo "h5cc112compat18($h5cc11218compat) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc112compat16 ]; then
    echo "h5cc112compat16($h5cc112compat16) not found or not executable.  Abort"
    exit 1
fi

if [ ! -x $h5cc110 ]; then
    echo "h5cc110($h5cc110) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc110compat18 ]; then
    echo "h5cc110compat18($h5cc11018compat) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc110compat16 ]; then
    echo "h5cc110compat16($h5cc110compat16) not found or not executable.  Abort"
    exit 1
fi

if [ ! -x $h5cc18 ]; then
    echo "h5cc18($h5cc18) not found or not executable.  Abort"
    exit 1
fi
if [ ! -x $h5cc18compat ]; then
    echo "h5cc18compat($h5cc18compat) not found or not executable.  Abort"
    exit 1
fi

if [ ! -x $h5cc16 ]; then
    echo "h5cc16($h5cc16) not found or not executable.  Abort"
    exit 1
fi

echo
echo "Testing scripts used:"
echo "h5ccdev = $h5ccdev"
echo "h5ccdevcompat112 = $h5ccdevcompat112"
echo "h5ccdevcompat110 = $h5ccdevcompat110"
echo "h5ccdevcompat18 = $h5ccdevcompat18"
echo "h5ccdevcompat16 = $h5ccdevcompat16"
echo "h5cc112 = $h5cc112"
echo "h5cc112compat110 = $h5cc112compat110"
echo "h5cc112compat18 = $h5cc112compat18"
echo "h5cc112compat16 = $h5cc112compat16"
echo "h5cc110 = $h5cc110"
echo "h5cc110compat18 = $h5cc110compat18"
echo "h5cc110compat16 = $h5cc110compat16"
echo "h5cc18 = $h5cc18"
echo "h5cc18compat = $h5cc18compat"
echo "h5cc16 = $h5cc16"


##################  MAIN  ##################

if ( TEST_H5A &&\
    TEST_H5D &&\
    TEST_H5E &&\
    TEST_H5F &&\
    TEST_H5G &&\
    TEST_H5O &&\
    TEST_H5P &&\
    TEST_H5R &&\
    TEST_H5S &&\
    TEST_H5T &&\
    TEST_H5Z); then
    EXIT_VALUE=0
else
    EXIT_VALUE=1
fi

##################  CLEANUP  ##################

CLEANUP
echo

exit $EXIT_VALUE

