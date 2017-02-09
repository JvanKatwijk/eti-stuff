eti-backend
------------

eti-backend is a program that in a limited way is able to interpret
ETI files, and extract program data from these files.

If you already installed Qt-DAB, then everything is there to install
the eti-backend as well


Operation is simple:
on the command line you may specify -F file for the input file.
If nothing is specified, stdin is taken.

Together with the eti-frontend, you can create a full dab receiver
by eti-frontend -O - |eti-backend


