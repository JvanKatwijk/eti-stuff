#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti-cmdline program
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
 *    along with eti-cmndline; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	This particular driver is a very simple wrapper around the
 * 	librtlsdr.  In order to keep things simple, we dynamically
 * 	load the dll (or .so). The librtlsdr is osmocom software and all rights
 * 	are greatly acknowledged
 */


#include	"rtl-sdr.h"
#include	"rtlsdr-handler.h"

#define	READLEN_DEFAULT	8192
//
//	For the callback, we do need some environment which
//	is passed through the ctx parameter
//
//	This is the user-side call back function
//	ctx is the calling task
static
void	RTLSDRCallBack (uint8_t *buf, uint32_t len, void *ctx) {
rtlsdrHandler	*theStick	= (rtlsdrHandler *)ctx;

	if ((theStick == NULL) || (len != READLEN_DEFAULT))
	   return;

	(void) theStick -> _I_Buffer -> putDataIntoBuffer (buf, len);
}
//
//	for handling the events in libusb, we need a controlthread
//	whose sole purpose is to process the rtlsdr_read_async function
//	from the lib.
void	controlThread (rtlsdrHandler *theStick) {
	rtlsdr_read_async (theStick -> device,
	                   (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                   (void *)theStick,
	                   0,
	                   READLEN_DEFAULT);
}
//
//	Our wrapper is a simple classs
	rtlsdrHandler::rtlsdrHandler (int frequency, int16_t ppmOffset,
	                              int16_t gain, bool autogain) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;

	this	-> frequency	= frequency;
	this	-> ppmOffset	= ppmOffset;
	this	-> theGain	= gain;
	this	-> autogain	= autogain;

	inputRate		= 2048000;
	libraryLoaded		= false;
	open			= false;
	_I_Buffer		= NULL;
	this	-> sampleCounter= 0;
	gains			= NULL;
	running			= false;

//
//	Ok, from here we have the library functions accessible
	deviceCount 		= rtlsdr_get_device_count ();
	if (deviceCount == 0) {
	   fprintf (stderr, "No devices found\n");
	   goto err;
	}

	deviceIndex = 0;	// default
//
//	OK, now open the hardware
	r			= rtlsdr_open (&device, deviceIndex);
	if (r < 0) {
	   fprintf (stderr, "Opening dabstick failed\n");
	   goto err;
	}

	open			= true;
	r			= rtlsdr_set_sample_rate (device, inputRate);
	if (r < 0) {
	   fprintf (stderr, "Setting samplerate failed\n");
	   goto err;
	}

	r		= rtlsdr_get_sample_rate (device);
	fprintf (stderr, "samplerate set to %d\n", r);
	rtlsdr_set_tuner_gain_mode (device, 0);

	gainsCount	= rtlsdr_get_tuner_gains (device, NULL);
	fprintf (stderr, "Supported gain values (%d): ", gainsCount);
	gains		= new int [gainsCount];
	gainsCount	= rtlsdr_get_tuner_gains (device, gains);
        rtlsdr_set_tuner_gain_mode (device, autogain);
	rtlsdr_set_tuner_gain (device,gains [theGain * gainsCount / 100]);

	_I_Buffer	= new RingBuffer<uint8_t>(1024 * 1024);
	return;

err:
	if (open)
	   rtlsdr_close (device);
	throw (33);
}

	rtlsdrHandler::~rtlsdrHandler	(void) {
	if (running) { // we are running
	   rtlsdr_cancel_async (device);
	   workerHandle. join ();
	}
	running	= false;
	if (open)
	   rtlsdr_close (device);
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (gains != NULL)
	   delete[] gains;
	open = false;
}

//
bool	rtlsdrHandler::restartReader	(int32_t freqency) {
int32_t	r;

	if (running)
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	r = rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	rtlsdr_set_freq_correction	(device, ppmOffset);
	rtlsdr_set_center_freq		(device, frequency);
	workerHandle = std::thread (controlThread, this);
	rtlsdr_set_tuner_gain_mode (device, autogain);
	rtlsdr_set_tuner_gain (device, gains [theGain * gainsCount / 100]);
	running	= true;
	return true;
}

void	rtlsdrHandler::stopReader		(void) {
	if (!running)
	   return;

	rtlsdr_cancel_async (device);
	workerHandle. join ();
	running	= false;
}
//
//	The brave old getSamples. For the dab stick, we get
//	size samples: still in I/Q pairs, but we have to convert the data from
//	uint8_t to DSPCOMPLEX *
int32_t	rtlsdrHandler::getSamples (DSPCOMPLEX *V, int32_t size) { 
int32_t	amount, i;
uint8_t	*tempBuffer = (uint8_t *)alloca (2 * size * sizeof (uint8_t));
//
	amount = _I_Buffer	-> getDataFromBuffer (tempBuffer, 2 * size);
	for (i = 0; i < amount / 2; i ++)
	    V [i] = DSPCOMPLEX ((float (tempBuffer [2 * i] - 128)) / 128.0,
	                        (float (tempBuffer [2 * i + 1] - 128)) / 128.0);
	return amount / 2;
}

int32_t	rtlsdrHandler::Samples	(void) {
	return _I_Buffer	-> GetRingBufferReadAvailable () / 2;
}

void	rtlsdrHandler::resetBuffer (void) {
	_I_Buffer -> FlushRingBuffer ();
}

int16_t	rtlsdrHandler::bitDepth	(void) {
	return 8;
}

