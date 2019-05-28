
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
4. HACKrf devices
5. prerecorded RAW input files (in format u8, \*.raw)
6. prerecorded wave files (in format s16le, \*.sdr)

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
(RTLSDR, SDRPLAY, AIRSPY, HACKRF, RAWFILES, WAVFILES)

If `-DDUMPING=ON` is added, the possibility for dumping the input to an ".sdr" 
file (note that an sdr-file is a ".wav" file, with a samplerate of 2048000 
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
2. `-d number`, where number indicates the number of seconds used to wait for
    time synchronization. If time synchronization cannot be achieved within
    the specified time (default 5 seconds) it is pretty unlikely that a
   DAB signal is in the selected channel.
3. `-O filename`, for specifying the file onto which the ETI frames are written,
   "-O -" indicates that the output is to be written to stdout. Note that
   not specifying the "-O" option also causes the output to be written
   to stdout.

4. `-R filename`, for dumping the input to a file as mentioned above. This
   option only makes sense when dumping is configures.

For use with one of the physical devices, one may set the following parameters

5. `-B ("L_BAND" | "BAND III")` for selecting the band. Default BAND III is chosen.

6. `-C channel`,  for selecting the channel to be set, e.g. 11C, default 11C
   is chosen

For use with rtlsdr based devices, one may use

7. `-G number`, for setting the gain with the device, a number between 0 .. 100,
   where 100 is the highest gain.

8. `-Q`, for setting the autogain with the device (assuming the device supports
   autogain setting)

9. `-P number`, for setting the ppm correction

10. `-I number`, for setting the device index. 

For use with sdrplay devices, one may use

7. `-G number`, for setting the if gain reduction, and

8. `-L number`, for setting the LNA state, and

9. `-Q`, for setting the autogain

10. `-P`, for setting the ppm correction

For use with HACKrf devices, one may use

7. `-L number`, for setting the lna gain, and

8. `-V number`, for setting the vga gain

9. `-P number`, for setting the ppm correction.

10. `-E`, for switching on the on board amplifier.
For use with AIRSPY devices, one may use

7. `-G number` for setting the gain (combined gain setting, range 0..21)

8. `-P number` for setting the ppm correction.

9. `-b` activates the bias tee (i.e. for use with an external amplifier)


For use with file input 

7. `-F filename`, the full pathname for the input file

8. `-E`, if selected the file will be reread after reaching eof.

For use with rtl_tcp

11. `-H hostname`, the hostname of the server to connect to, default 127.0.0.1

12. `-I port`, the port to listen to, default 1234


### Writing to eti files

Example:

	eti-cmdline-xxx -C 11C -G 80 > "11C_$(date +%F_%H%M).eti"
	
will write an ETI file (with date and time in the filename) to the current directory.

### Piping to dablin

You can use dablin or dablin_gtk from https://github.com/Opendigitalradio/dablin as a frontend by running
     
	eti-cmdline-xxx -C 11C -G 80 | dablin_gtk -L
     
where xxx refers to the input device being supported, one of (`rtlsdr`, `sdrplay`, `airspy`, `hackrf`, `rawfiles`, `wavfiles`).

## Copyright

	Copyright (C)  2016, 2017, 2018, 2019
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Programming

The eti-cmdline software is made available under the GPL-2.0.
All SDR-J software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

