"Eti-stuff" consists of three programs and is an attempt to
understand the eti structure as defined in ETS 300 799.
eti-frontend and eti-backend are based on the Qt-DAB software,
with code included from dabtools to actually generate the eti frames.
eti-cmdline is based on the dab-cmdline software,

The eti-frontend and eti-cmdline programs take as input a sample stream
and create an eti file, while eti-backend is interpreting the eti file.

Copyright

Copyright Jan van Katwijk (J.vanKatwijk@gmail.com).

This software is part of the  SDR-J, SDR-J is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version, taking into account the licensing conditions of the parts of the
software that are derived from wotk of others.

This software uses parts of dabtools.
Excerpt from the README of dabtools  reads
"dabtools is written by Dave Chapman <dave@dchapman.com>
Large Parts of the code in both eti-frontend and eti-backend are copied verbatim (or with trivial
modifications) from David Crawley's OpenDAB and hence retain his copyright."

Obviously, the copyrights for the parts copied (or directly derived)
from the dabtools remain with Dave Chapman.

The set consists (currently) of two programs (both under continuous development)

a. eti-frontend, which is a stripped Qt-DAB version that
   generates eti-frames as output.
   The GUI is - obviously - based on Qt-DAB. There is an additional
   command line option "-O -", that will cause the ETI files to be
   send to stdout.
   The "normal" way of working is to have an input device selected, select
   a channel, press the start button, and as soon as an ensemble is
   identified, and the GUI shows the names of the programs, to press
   the button "eti is waiting", after which it will send the eti frames
   to the selected output device.
b. eti-backend, which is a stripped Qt-DAB version which takes
   eti-frames as input. A command line option (-s) is to specify stdin as
   input file,
   if none is specified, the program will show a widget for selecting an
   input file.
   The GUI is simple, it will show the name of the ensemble and the names
   of programs. Selecting a program is as in the Qt-DAB software.
   It will start reading after pressing the start button.
c. eti-cmdline, which is derived from the dab-cmdline program and shares
   the relevant commnd line parameters.

So, if you want to emulate a full dab-receiver, then just do

    eti-frontend -O - | eti-backend -s

If you want to run the backend there are two options,
a. one is to take input from stdin, indicated by a "-s" as command line option.
b. if no "-s" is specified on the command line, you will get a menu when 
starting the program, to select a file.

Since the software is derived from the Qt-DAB and the dab-cmdline
software, it supports - if so configured - the same devices,
i.e. rtlsdr based dabsticks, airspy and sdrplay
(and the GUI based programs prerecorded files).

Disclaimer

The software is under development and most likely contains errors.
SDR-J is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.



