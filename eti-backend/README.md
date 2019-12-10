eti-backend
------------

Note that this eti-backend is pretty experimental and does not perform
yet as it should be.

eti-backend is a program that in a limited way is able to interpret
ETI files, and extract program data from these files.

If you already installed Qt-DAB, then everything is there to install
the eti-backend as well

Operation is simple:
on the command line you may specify "-s file" for using the named file
for input, "-s -" denotes taking stdin as input file.
If nothing is specified, a menu will be presented for selecting an input file.

Together with the eti-frontend, you can create a full dab receiver
by eti-frontend -O - |eti-backend -s -


