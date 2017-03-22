# eti-stuff

eti-stuff is an attempt to understand the ETI structure as defined in ETS 300 799. It is based on the Qt-DAB software with code included from dabtools to actually generate the ETI frames.

It consists of two programs, one generating a stream of ETI frames, the other one reading and interpreting the ETI frames, such that by piping the output from the one into the other one has a more or less complete DAB receiver.


## Contents


### eti-frontend

Is a stripped Qt-DAB version that generates ETI frames as output or save them to a `*.eti` file. The GUI is - obviously - based on Qt-DAB.


#### Syntax

   `eti-frontend -O -`

will cause the ETI frames to be sent to stdout, whereas

   `eti-frontend -O`

will show a file save menu for selecting an output file (`*.eti`).

Then select an input device or a `*.raw` or `*.sdr` file, choose the VHF III or L-Band channel carrying the DAB or DAB+ signal, and press the start button. After it, an ensemble will be identified and the GUI shows the names of the services. Please wait until the quality indicator shows the value 100. Then press the button "eti is waiting" which then will send the ETI frames to the selected output device resp. to a file.
   
### eti-backend

Is a stripped Qt-DAB version which takes ETI frames as input or from a `*.eti` file.


#### Syntax

   `eti-backend -`

specifies stdin as input, whereas 

   `eti-backend` 

will show a file open menu for selecting an input file (`*.eti`).

The GUI is simple, it will show the name of the ensemble and the names of the services. Selecting a service is the same as in the Qt-DAB software.

It will start reading after pressing the start button.

So, if you want to emulate a full dab-receiver, then just place both commands together, separated by a pipe:
   
   ```
   eti-frontend -O - | eti-backend -
   ```
  
Since the software is derived from the Qt-DAB software, it supports therefore - if so configured - the same devices, i.e. dabsticks, Airspy (mini) and sdrplay (and prerecorded files of course).


## Windows

For Windows there is a compiled version to be found at <http://sdr-j.tk/windows-bin.zip> . Please note that stdin and stdout are not supported, please use `*.eti` files instead.


## Linux

For prerequisites and building process, please see <https://github.com/JvanKatwijk/qt-dab>

Don't forget to change `eti-frontend.pro` file first by commenting out devices you do not own. For building you could use Qt Creator as well.


## Disclaimer

The software is under development and most likely contains errors.

eti-stuff is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.


## Copyright

Copyright Jan van Katwijk <J.vanKatwijk@gmail.com>.

This software is part of the Qt-DAB, Qt-DAB is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version, taking into account the licensing conditions of the parts of the software that are derived from wotk of others.

This software uses parts of dabtools.
Excerpt from the README of dabtools reads

"dabtools is written by Dave Chapman <dave@dchapman.com>
   
Large Parts of the code in both eti-frontend and eti-backend are copied verbatim (or with trivial modifications) from David Crawley's OpenDAB and hence retain his copyright."

Obviously, the copyrights for the parts copied (or directly derived) from the dabtools remain with Dave Chapman.
