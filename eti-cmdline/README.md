
# eti-cmdline

eti-cmdline is an experimental program for creating a stream of ETI frames 
from a selected DAB input channel. The program is fully command line driven.

## Table of Contents

* [Supported Devices](#supported-input-devices)
* [Installation](#installation)
* [Configuring](#configuring)
* [Command line parameters](#command-line-parameters)
* [Copyright](#copyright)

## Supported input devices

The supported input devices are:

1. Dabsticks (rtlsdr) that are supported by the osmocom driver software
2. SDRplay devices (RSP1 and RSP2)
3. AIRSpy devices
4. prerecorded RAW input files (in format u8, \*.raw)
5. prerecorded wave files (in format s16le, \*.sdr)

## Installation

For compiling and installing the software `cmake` needs to be installed. 

Required libraries - apart from those needed to support the device - are:

* fftw3
* libsndfile
* libsamplerate

## Configuring

The "normal" way for configuring and installing is 

   	mkdir build
  	cd build
   	cmake .. -DXXX=ON  [-DDUMPING=ON]
   	make

where XXX refers to the input device being supported, one of 
(RTLSDR, SDRPLAY, AIRSPY, RAWFILES, WAVFILES)

If `-DDUMPING=ON` is added, the possibility for dumping the input to a ".sdr" 
file (note that an sdr file is a ".wav" file, with a samplerate of 2048000 
and short int values).

The resulting program is named `eti-cmdline-XXX`, for XXX see above.

The command `(sudo) make install` will install the created executable in 
`/usr/local/bin` unless specified differently (note that it requires root permissions)


## Command line parameters

Once the executable is created, it needs to be told what channel you want to be read in and converted.

General parameters are

1. `-D number`, where number indicates the number of seconds used to collect information 
   on the ensemble. The default value is 10. In 9 out of 10 cases, if there is not an 
   ensemble detected within 10 seconds, it will never be detected.
   
   Note that as soon as the software detects a DAB like signal, a message
   is printed (which can arrive as fast as in 1 or 2 seconds).

2. `-O filename`, for specifying the file onto which the ETI frames are written,
   "-O -" indicates that the output is to be written to stdout. Note that
   not specifying the "-O" option also causes the output to be written
   to stdout.

For use with one of the physical devices, one may set the following parameters

3. `-B ("L_BAND"| "BAND III")` for selecting the band. Default BAND III is chosen.

4. `-C channel`,  for selecting the channel to be set, e.g. 11C, default 11C
   is chosen

5. `-G number`, for setting the gain with the device, a number between 0 .. 100,
   where 100 is the highest gain.

6. `-Q`, for setting the autogain with the device (assuming the device supports
   autogain setting)

7. `-R filename`, for dumping the input to a file as mentioned above. This
   option only makes sense when dmping is configures.

For use with file input 

8. `-F filename`, the full pathname for the input file

9. `-E`, is selected the file will be reread after reaching eof.

For use with rtl_tcp

10. `-H hostname`, the hostname of the server to connect to, default 127.0.0.1

11. `-I port`, the port to listen to, default 1234


### Writing to eti files

Example:

	eti-cmdline-xxx -C 11C -G 80 > "11C_$(date +%F_%H%M).eti"
	
will write an ETI file (with date and time in the filename) to the current directory.

### Piping to dablin

You can use dablin or dablin_gtk from https://github.com/Opendigitalradio/dablin as a frontend by running
     
	eti-cmdline-xxx -C 11C -G 80 | dablin_gtk -L
     
where xxx refers to the input device being supported, one of (`rtlsdr`, `sdrplay`, `airspy`, `rawfiles`, `wavfiles`).

## Copyright

	Copyright (C)  2016, 2017, 2018
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Programming

The eti-cmdline software is made available under the GPL-2.0.
All SDR-J software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
