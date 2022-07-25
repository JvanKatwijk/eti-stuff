#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti-cmdline program
 *
 *    eti-cmdline is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti-cmdline is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti-cmdline; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<time.h>
#include	"wavfile-handler.h"

static inline
int64_t		getMyTime	(void) {
struct timeval	tv;

	gettimeofday (&tv, NULL);
	return ((int64_t)tv. tv_sec * 1000000 + (int64_t)tv. tv_usec);
}

	wavfileHandler::wavfileHandler (std::string filename,
	                                bool continue_on_eof,
	                                inputstopped_t inputStopped):
	                                     _I_Buffer (16 * 32768) {
SF_INFO *sf_info;

	sf_info         = (SF_INFO *)alloca (sizeof (SF_INFO));
        sf_info -> format       = 0;
        filePointer     = sf_open (filename.c_str (), SFM_READ, sf_info);
        if (filePointer == NULL) {
           fprintf (stderr, "file %s no legitimate sound file\n",
                                        filename. c_str ());
           throw (24);
        }
        if ((sf_info -> samplerate != 2048000) ||
            (sf_info -> channels != 2)) {
           fprintf (stderr,
	            "%s is not a recorded dab file, sorry\n",
	                         filename. c_str ());
           sf_close (filePointer);
           throw (25);
        }
	this	-> continue_on_eof	= continue_on_eof;
	this	-> inputStopped		= inputStopped;
	currPos		= 0;
	eof		= false;
	run. store (false);
}

	wavfileHandler::~wavfileHandler (void) {
	if (run. load ()) {
	   run. store (false);
	   threadHandle. join ();
	}
	sf_close (filePointer);
}

bool	wavfileHandler::restartReader	(int32_t freq) {
	if (run. load ())
	   return true;
	(void)freq;
	run. store (true);
        threadHandle    = std::thread (&wavfileHandler::runRead, this);
	return true;
}

void	wavfileHandler::stopReader	(void) {
	if (run. load ()) {
	   run. store (false);
	   threadHandle. join ();
	}
}
//
//	size is in I/Q pairs
int32_t	wavfileHandler::getSamples	(std::complex<float> *V,
	                                 int32_t size) {
	if (filePointer == NULL)
	   return 0;
	return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	wavfileHandler::Samples () {
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	wavfileHandler::runRead (void) {
int32_t	t, i;
int32_t	bufferSize	= 32768;
std::complex<float> bi [bufferSize];
int64_t	period;
int64_t	nextStop;
	run. store (true);

	period		= (32768 * 1000) / 2048;	// full IQś read
	fprintf (stderr, "Period = %ld\n", period);
	nextStop	= getMyTime ();
	while (run. load ()) {

	   while (_I_Buffer. WriteSpace () < bufferSize) {
	      if (!run. load ())
	         break;
	      usleep (1000);
	   }

	   nextStop += period;
	   t = readBuffer (bi, bufferSize);
	   if (t < bufferSize) {
	      for (i = t; i < bufferSize; i ++)
	          bi [i] = std::complex<float> (0, 0);
	      t = bufferSize;
	   }

	   _I_Buffer. putDataIntoBuffer (bi, bufferSize);
	   if (nextStop - getMyTime () > 0)
	      usleep (nextStop - getMyTime ());
	}
	fprintf (stderr, "thread stops\n");
}
/*
 *	length is number of uints that we read.
 */
int32_t	wavfileHandler::readBuffer (std::complex<float> *data,
	                            int32_t length) {
int32_t	i, n;
float	temp [2 * length];

	n = sf_readf_float (filePointer, temp, length);
	if (n < length) {
	   if (!continue_on_eof) {
	      if (eof)
	         return 0;
	      eof = true;
	      inputStopped ();
	      return 0;
	   }
	   sf_seek (filePointer, 0, SEEK_SET);
	   fprintf (stderr, "End of file, restarting\n");
	}

	for (i = 0; i < n; i ++)
	   data [i] = std::complex<float> (temp [2 * i], temp [2 * i + 1]);
	return	n;
}

