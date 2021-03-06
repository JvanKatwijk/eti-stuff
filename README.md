# eti-stuff

"eti-stuff" is an attempt to understand the eti structure as defined in ETS 300 799 for Digital Audio Broadcasting (DAB and DAB+).

[eti-cmdline](#eti-cmdline) is based on the dab-cmdline software  with code
included from [dabtools](https://github.com/Opendigitalradio/dabtools) to actually decode the eti frames.
It is - as the name suggests - a command line version.

eti-cmdline now supports a whole range of device (the device is for cmake command, see below):

 - RTLSDR: for DABStickes based on Realtek 2832 chipset,
 - AIRSPY: for Airspy R2 and Airspy mini devices (not for Airspy HF+),
 - SDRPLAY: for SDRPlay RSP devices using the 2.13 SDRplay library,
 - SDRPLAY_V3: for SDRPlay RSP devices using the 3.06/7 SDRplay library,
 - PLUTO: for Adalm Pluto devices,
 - HACKRF: for HackRF devices,
 - LIMESDR: for LimeSDR devices
 - RTL_TCP: for rtl_tcp input (and multiple DABsticks support),
 - RAWFILES: for 8bit unsigned raw files
 - WAVFILES: for 16bit wave files
 - XML_FILES: for uff and xml files, created by Qt-DAB or Qirx

When constructing, select the input device of choice in the CMake command, for example

      cmake -DRTLSDR=ON  # for DABSticks
      cmake -DRAWFILES=ON # for u8 raw files
      make
      sudo make install
     
By piping the output from eti-cmdline into dablib_gtk, a more or less complete DAB receiver exists.

      
You can use dablin or dablin_gtk from https://github.com/Opendigitalradio/dablin by running
      
      eti-cmdline-xxx -C 11C -G 80 | dablin_gtk
      
where xxx refers to the input device being supported, one of (`rtlsdr`, `sdrplay`, `sdrplay-v3`, `pluto`, `airspy`, `hackrf`, `limesdr`, `rawfiles`, `wavfiles`, `xml_files`, `rtl_tcp`).
      
# Disclaimer

The software is under development and most likely contains errors.

eti-stuff is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.


# Copyright

Copyright Jan van Katwijk <J.vanKatwijk@gmail.com>.
Lazy Chair Computing

This software is part of the Qt-DAB collection, Qt-DAB is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version, taking into account the licensing conditions of the parts of the software that are derived from wotk of others.

This software uses parts of dabtools. Excerpt from the README of dabtools reads

"dabtools is written by Dave Chapman <dave@dchapman.com>
   
Parts of the code in eti-backend are copied verbatim (or with trivial modifications) from David Crawley's OpenDAB and hence retain his copyright."

Obviously, the copyrights for the parts copied (or directly derived) from the dabtools remain with Dave Chapman.
