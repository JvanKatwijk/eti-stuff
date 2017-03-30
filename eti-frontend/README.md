
eti-frontend
------------

eti-frontend is a program that in a limited way is able to generate
ETI files from DAB input.

If you already installed Qt-DAB, then everything is there to install
the eti-backend as well

Operation is simple:
on the command line you may specify -O file for the output file.
"-O -" sends the output to standard output. If no output file
is specified, output will be sent to standard output.

After starting the program, and selecting a channel, one may press
the button that says "eti waits", after pressing this button
the input is mapped upon ETI output.

Together with the eti-backend, you can create a full dab receiver
by eti-frontend -O - |eti-backend -s -


