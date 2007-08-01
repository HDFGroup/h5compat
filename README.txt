

============================  Purpose / Main Idea:  ===========================

	The script h5compat.sh is designed to test compatibility between the
v1.6 and v1.8 libraries of hdf5.  The script uses h5cc from both versions (v1.6
and v1.8) of the hdf5 library to compile programs and run tests.  First a 
standard hdf5 file is created using v1.6.  That file is then modified in some
way using the v1.8 library.  The modified file is then read by v1.6 and v1.8 to
identify compatibility problems.



==================================  Details:  =================================

 GENERATING THE FILE

	The program gen_compat.c is compiled using the v1.6 library and used to
create the hdf5 file compat.h5.

compat.h5 has the structure shown here:

                       /
     |           |        |      |      |
     g1          g2       g3     g4     g5
    / \          |        |      |      |
 g1.1 g1.2     dtype1   hlink2  dset2  slink1
  |     |     (dset1)   (dset2)        (dset2)
 dset1 hlink1
       (dset1)




 MODIFYING THE FILE

	There are many programs that are designed to modify compat.h5.  These
programs are contained in the folder "tests/".  Each test program is compiled
using the v1.8 library and tests a different element of the v1.8 format or a
different affect that the new format might have on version compatibility.

	Each test is run seperately, and the original file, compat.h5, is
re-generated with v1.6 before each test is run.




 READING / CHECKING THE FILE

	To check if the modified file is still readable and compatibile with
the v1.6 library, the file is read using read_compat.c (compiled with v1.6).
read_compat.c generates a file called errors.log, which contains the results of
every attempt to read part of the file.  Each line in errors.log contains 4 
parts.  First, either Error or Passed.  Second, the type of object trying to be
read (file, group, dataset, etc.).  Third, the name of the object being read
(g1, dset1, hlink1, etc.).  Fourth, an optional comment about the specific read
attempt.

The entry would look something like this:
...
Passed: dset hlink2 (through group g3)
Error: group g4 (null)
...

	This errors.log file is then compared to a pre-existing file containing
the expected output.  If the two files match exactly, the next test will be
run.

	If the two files do not match, an error message will be displayed, the
tests will stop, and a new file called CompatibilityError.log will be created.
This file contains the test name and both the recieved output (from errors.log)
and the expected output (found in a pre-existing file).  This file is intended
to be helpful in debugging the problem.




==============================  More Knowledge:  ==============================

more is coming... I promise
