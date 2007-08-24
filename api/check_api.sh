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

    # Check if output from test program is the same as expected output
    cmp -s $actual $expected
    ((ret = $?))

    # Check if output didn't match expected output
    if ((ret != 0)); then
	
        # Output file doesn't exist
        if ((ret == 2)); then
            # with the -a flag a new expected file will be created
            if [[ $AddExpected == "on" ]]; then
                echo "Expected output was not found."
                echo "Adding current output as expected output:"
                echo
                cat $actual | tee $expected
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
            diff $actual $expected

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
            cat $actual

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
    if (($? == 0)); then
#        echo "Testing with: $1"
        ./a.out > $2.out
#        ./a.out
        if (($? != 0)); then
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
    TESTING "actual 1.6.x library"
    BUILD $h5cc16 $1 "v16"

    TESTING "1.8.x library built in 1.6.x compatibility mode"
    BUILD $h5cc18compat $1 "v18"

    TESTING "normal 1.8.x library build, with 1.6.x compatibility macro"
    BUILD "$h5cc18 -DH5_USE_16_API" $1 "v18"
}

# Build test program with different API routine versions overridden
#
TESTAPI()
{
    # Create aliases for the parameters
    testname=$1
    suffix=$2
    options=$3

    TESTING "normal 1.8.x, with: $options"
    BUILD "$h5cc18 $options" $testname "v18-$suffix"
}

# Runs tests for H5E API
#
TEST_H5E()
{
    echo
    echo "################# Testing H5E API #################"

    # Run "entire library API" tests
    TEST test_h5e

    # Run tests for overriding version of individual API routines
    TESTAPI test_h5e H5Eclear1 "-DH5Eclear_vers=1"
    TESTAPI test_h5e H5Eclear2 "-DH5_USE_16_API -DH5Eclear_vers=2"
    TESTAPI test_h5e H5Eauto1 "-DH5E_auto_t_vers=1 -DH5Eget_auto_vers=1 -DH5Eset_auto_vers=1"
    TESTAPI test_h5e H5Eauto2 "-DH5_USE_16_API -DH5E_auto_t_vers=2 -DH5Eget_auto_vers=2 -DH5Eset_auto_vers=2"
    TESTAPI test_h5e H5Epush1 "-DH5Epush_vers=1"
    TESTAPI test_h5e H5Epush2 "-DH5_USE_16_API -DH5Epush_vers=2"
    TESTAPI test_h5e H5Ewalk1 "-DH5Ewalk_vers=1"
    TESTAPI test_h5e H5Ewalk2 "-DH5_USE_16_API -DH5Ewalk_vers=2"
    TESTAPI test_h5e H5Eprint1 "-DH5Eprint_vers=1"
    TESTAPI test_h5e H5Eprint2 "-DH5_USE_16_API -DH5Eprint_vers=2"
}


##################  INITIALIZE SCRIPT INFO  ##################


# Define compile scripts to use
h5cc18="/mnt/scr1/pre-release/hdf5/v180/kagiso/bin/h5cc"
h5cc18compat="/mnt/scr1/pre-release/hdf5/v180-compat/kagiso/bin/h5cc"
h5cc16="/mnt/scr1/pre-release/hdf5/v16/kagiso/bin/h5cc"

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
echo "h5cc18 = $h5cc18"
echo "h5cc18compat = $h5cc18compat"
echo "h5cc16 = $h5cc16"


##################  MAIN  ##################

TEST_H5E

##################  CLEANUP  ##################

CLEANUP
echo

exit 0

