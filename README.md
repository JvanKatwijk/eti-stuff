"Eti-stuff" consists of two programs and is an attempt to
understand the eti structure as defined in ETS 300 799.
It is based on the DAB-rpi software, with code included
from dabtools to actually generate the eti frames.

It consists of two programs, one generating a stream of ETI frames,
the other one reading and interpreting the ETI frames, such that
by piping the output from the one into the other one has a
more or less complete dab receiver.

Copyright

Copyright Jan van Katwijk (J.vanKatwijk@gmail.com).

This software is part of the  SDR-J (JSDR), SDR-J is free software; you can redistribute it and/or modify
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

a. eti-frontend, which is a stripped dab-rpi version which
   generates eti-frames as output.
   GUI is - obviously - based on dab-rpi. There is an additional
   command line option "-O -", that will cause the ETI files to be
   send to stdout.
   The "normal" way of working is to have an input device selected, select
   a channel, press the start button, and as soon as an ensemble is
   identified, and the GUI shows the names of the programs, to press
   the button "eti is waiting", after which it will send the eti frames
   to the selected output device.
b. eti-backend, which is a stripped dab-rpi version which takes
   eti-frames as input. A command line option is to specify an input file,
   if none is specified, the program will take its input from stdin.
   The GUI is simple, it will show the name of the ensemble and the names
   of programs. Selecting a program is as in the DAB-rpi software. It will start reading
   after pressing the start button.

So, if you want to emulate a full dab-receiver, then just do

    eti-frontend -O - | eti-backend



Since the software is derived from the DAB-rpi software, it supports
- if so configured - the same devices, i.e. dabsticks, airspy and sdrplay
(and prerecorded files of course).

Disclaimer

The software is under development and most likely contains errors.
SDR-J is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.



