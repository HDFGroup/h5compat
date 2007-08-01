#! /bin/sh

#  This file:  compat.sh
# Written by:  Peter Laird
#       Date:  July 16, 2007
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                       #
# This script will:                                                     #
#	- run compatibility tests using 2 different libraries		#
#                                                                       #
#                                                                       #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


h5cc18="/mnt/scr1/pre-release/hdf5/v180/kagiso/bin/h5cc"
h5cc16="/mnt/scr1/pre-release/hdf5/v16/kagiso/bin/h5cc"

ErrorFile="CompatibilityError.log"

####  Read with v1.6 ####

read16()
{
    $h5cc16 read_compat.c
    if (($? == 0))
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
    $h5cc18 read_compat.c
    if (($? == 0))
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

#### Check Errors ####

CheckErrors()
{
    expected=$1"-expected"
    cmp -s errors.log tests/expected/$expected
    if (($? == 0))
    then
	echo "Test ran as expected"
    else
	echo "!!! Error: There was an error running this test !!!"
	echo "          check ./CompatibilityError.log"
	echo


        echo "Test: $1" > $ErrorFile

        echo >> $ErrorFile
        echo "###########################################################" >> $ErrorFile
        echo >> $ErrorFile
        echo "Difference in output:" >> $ErrorFile
	diff errors.log tests/expected/$expected >> $ErrorFile

        echo >> $ErrorFile
        cat tests/expected/$expected >> $ErrorFile
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
    $h5cc18 tests/$Test
    ./a.out
    read16
    read18
    CheckErrors $1
    rm errors.log
}



##################  MAIN  ##################

$h5cc16 -o gen_compat.out gen_compat.c
if (($? == 0))
then
    RunTest t_newfile
    RunTest t_newgroup
    RunTest t_newdata
    RunTest t_newlink
    RunTest t_newtype
    RunTest t_newatts
    RunTest t_latest_mod_data
    RunTest t_latest_mod_attr
else
    echo "messed up compiling gen_compat.c"
fi

rm a.out
rm gen_compat.out
rm *.o
rm compat.h5
echo

exit 0

