
# ETI-CMDLINE in C++ 

ETI-CMDLINE is a DAB front end, mapping an incoming DAB stream to an eti file. The program is derived from the existing eti-frontend, however no GUI is provided.

To allow the construction of various GUI's, the functionality is implemented as a library and a separate "main" program.

The library provides through a simple API entries for the functionality through some simple calls, while a callback function provides some communication back from the library to the gui.

To show the use of the library, a - functioning - command-line handler is included in this repository, written in C++. The sources can be found in the directory "eti-cmd".

# Command-line Parameters for the C++ version

The C++ command line program can be compiled using cmake. 

Of course, the eti library (see a next section) should have been installed. Libraries that are further needed are the fftw3 library.

The sequence to create the executable then is (after changing the working directory to the directory "eti-cmd")

	mkdir build
	cd build
	cmake ..
	make

The program accepts the following command line parameters:

	-B Band
selects the DAB band (default Band III),

	-M Mode
selects the DAB Mode (default Mode 1),

	-C the channel
the default is 11C, the channel I am listening to mostly,

	-G the gain 
to be applied on the device, a value in the range from 1 .. 100.

The value will be translated to an acceptable value for the device. In case the gain is table driven, as in the case of a dabstick, a value of e.g. 75 is translated into the element on three quarters of the table (basic assumption is that the table elements are more or less linear). For e.g. the Airspy the values are mapped upon the range 0 .. 21 of the sensitivity slider. 

For e.g. the sdrplay, it is simple since there are 101 attenuation values. Setting the gain to N, implies setting the attenuation to 101 - N.

For each of the parameters there is a default, i.e., if the command

	./eti-cmdline
	
is given, the assumptions are 

1. the Mode is "1",
2. the band is "BAND III", that the channel selected is "11C",
 
Note again, that the choice for the input device was fixed when creating the dab-library.

An example of a full specification of the command line is

	./eti-cmdline -M 1 -B "BAND III" -C 12C" -G 80
The program - when started - will try to identify a DAB datastream in the selected channel (here channel 12C). If a valid DAB datastream is found, the names of the programs in the ensemble will be printed. If - after a given amount of time - no ensemble could be found, execution will halt, if the ensemble was identified, the data for the selected program (here Radio 4) will be decoded.
	

# Creating the library

Note that - to keep things simple - the supported device, i.e. one of dabstick, airspy or sdrplay, is "compiled in" the library. The library can be created by  the commands (to be executed within the directory "eti-library"

	mkdir build 
	cd build 
	cmake .. -DXXXX=ON
	make 
	sudo make install

substitute for XXXX the name of the device that is to be supported, either "DABSTICK", "SDRPLAY" or "AIRSPY"

The CMake file contains a command line option to include C++11 support.

# Needed Libraries

Libraries (together with the "development" or ".h" files) needed for creating the library are

	libfftw3f
	libusb-1.0

and - obviously, the libraries for the device included.

# The API

The API is the API of the eti library, it is a simple API with only a few functions. It is advised to look at the example program (especially the "main" program) to see how the API is used.

The API has three elements,

1. typedefinition of the dabBand
2. the type specifications of the callback function,
3. a specification of the functions callable in the API.

		enum dabBand {
	 	BAND_III	= 0,
	 	L_BAND	= 1
		};


# The Callback Function

The ensemble  - when discovered in the selected channel - is presented as a list of strings. That list is handed over to the user of the library by a user defined callback function. The boolean parameter in this function tells whether or not an ensemble was found. If no ensemble was found, it is (almost) certain that there is no decent signal.

The type of the callback function providing the program names as appearing in the ensemble, should be conformant to
 
	typedef void (*cb_ensemble_t)(std::list<std::string>, bool);

Note that this function is *required* to be provided for,


# API-Functions	

The initialization function takes as parameters the immutable values for
 
1. the dabMode, is just one of 1, 2 or 4 (Mode 3 is not supported),
2. the dabBand, see the type above,
3. the time - in seconds - that is/can be used to detect an ensemble
4. the Filepointer for the file to which the output is written

Note that by creating a eti-library, you already selected a device, so the handler software for the device is part of the library.

The initialization function returns a non-NULL handle when the device could be opened for delivering input, otherwise it returns NULL.
 
	void	*eti_initialize	(uint8_t,	// dab Mode
	                         dabBand,	// Band
	                         int16_t,	// waiting time
	                         FILE *		// outputfile
	                         );
	
The return value, i.e. the handle, is used to identify the library instance in the other functions defined in the API.
  
The gain of the device can be set and changed to a value in the range 0 .. 100 using the function dab_Gain. The parameter value is mapped upon an appropriate value for the device
  
	void	eti_Gain	(void *handle, uint16_t);	

The function eti_Channel maps the name of the channel onto a frequency for the device and prepares the device for action. If the software was already running for another channel, then the thread running the software will be halted first.
 
     bool	eti_Channel	(void *handle, std::string);
 
The function returns - pretty obvious - true if the string for the channel could be recognized and the device could be set to the associated frequency, if not the function returns false (e.g. "23C" is a non-existent channel in Band III).

The function eti_run will start a separate thread, running the dab decoding software at the selected channel. Behaviour is undefined if no channel was selected. If after some time, DAB data, i.e. an ensemble, is found, then the function passed as callback is called with the boolean parameter set to true, and the std::list of strings, representing the names of the programs in that ensemble. If no data was found, the boolean parameter is set to false, and the list is empty. 
 
Note that the thread executing the dab decoding will continue to run.
 
     void	eti_run		(void *handle, cb_ensemble_t);

One might check whether or not the thread is still running by

     bool	is_running	(void *handle);

The function stop will stop the running of the thread that is executing the dab decoding software

     void	dab_stop	(void *handle);

The exit function will close down the library software and will set the handle, the address of which is passed as parameter, to NULL.
 
     void	dab_exit	(void **handle);

# Copyright

Copyright (C)  2016, 2017
Jan van Katwijk (J.vanKatwijk@gmail.com)
Lazy Chair Programming

The dab-library software is made available under the GPL-2.0.

All SDR-J software, among which dab-library is one - is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 	GNU General Public License for more details.
