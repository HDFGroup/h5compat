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


h5cc18="/mnt/scr1/plaird/hdf5/vdev/kagiso-1_8/hdf5/bin/h5cc"
h5cc16="/mnt/scr1/plaird/hdf5/vdev/kagiso-1_6/hdf5/bin/h5cc"

####  Read with v1.6 ####

read16()
{
    $h5cc16 read_compat.c
    if (($? == 0))
    then
        echo
	echo "========= Reading with v1.6 ========="
	echo
        ./a.out 2>/dev/null
        grep "Error:" errors.log
        if (($? == 1))
        then
            echo "No Errors were found"
        fi
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
        echo
        echo "========= Reading with v1.8 ========="
        echo
        ./a.out 2>/dev/null
        grep "Error:" errors.log
        if (($? == 1))
        then
            echo "No Errors were found"
        fi
    else
        echo "messed up compiling read_compat.c with v1.8"
    fi
}

#### Run test ####

RunTest()
{
    Test=$1

    echo
    echo "###########################  $Test  ###########################"
    ./gen_compat.out
    $h5cc18 tests/$Test
    ./a.out
    read16
    read18
}



##################  MAIN  ##################


$h5cc16 -o gen_compat.out gen_compat.c
if (($? == 0))
then
    RunTest t_newfile.c
    RunTest t_newgroup.c
    RunTest t_newdata.c
    RunTest t_newlink.c
    RunTest t_newtype.c
else
    echo "messed up compiling gen_compat.c"
fi

rm a.out
rm gen_compat.out
rm *.o
rm errors.log
# rm compat.h5
echo

exit 0

