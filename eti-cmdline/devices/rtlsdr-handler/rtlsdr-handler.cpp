#
/*
 *    Copyright (C) 2010, 2011, 2012, 2013
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
	rtlsdrHandler::rtlsdrHandler (int	frequency,
	                              int16_t	ppmOffset,
	                              int16_t	gain,
	                              bool	autogain,
	                              int16_t	deviceIndex) {
int16_t	deviceCount;
int32_t	r;
bool	open;

	this	-> frequency	= frequency;
	this	-> ppmOffset	= ppmOffset;
	this	-> theGain	= gain;
	this	-> autogain	= autogain;
	this	-> deviceIndex	= deviceIndex;

	inputRate		= 2048000;
	open			= false;
	_I_Buffer		= NULL;
	gains			= NULL;
	running. store (false);
//
//	Ok, from here we have the library functions accessible
	deviceCount 		= rtlsdr_get_device_count ();
	if (deviceCount == 0) {
	   fprintf (stderr, "No devices found\n");
	   goto err;
	}
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

	gainsCount      = rtlsdr_get_tuner_gains (device, NULL);
        fprintf (stderr, "Supported gain values (%d): ", gainsCount);
        gains           = new int [gainsCount];
        gainsCount      = rtlsdr_get_tuner_gains (device, gains);
        for (int i = 0; i < gainsCount; i ++)
           fprintf (stderr, "%d.%d ", gains [i] / 10, gains [i] % 10);
        fprintf (stderr, "\n");

        if (ppmOffset != 0)
           rtlsdr_set_freq_correction (device, ppmOffset);

        if (autogain)
           rtlsdr_set_agc_mode (device, 1);

        fprintf (stderr, "effective gain: gain %d.%d\n",
	                             gains [theGain * gainsCount / 100] / 10,
	                             gains [theGain * gainsCount / 100] % 10);
        rtlsdr_set_tuner_gain (device, gains [theGain * gainsCount / 100]);

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

	running. store (false);
	rtlsdr_close (device);
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (gains != NULL)
	   delete[] gains;
}
//
bool	rtlsdrHandler::restartReader	(int32_t freqency) {
int32_t	r;

	if (running. load ())
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	r = rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	rtlsdr_set_freq_correction	(device, ppmOffset);
	rtlsdr_set_center_freq		(device, frequency);

	workerHandle = std::thread (controlThread, this);
	rtlsdr_set_tuner_gain (device, gains [theGain * gainsCount / 100]);
        if (autogain)
           rtlsdr_set_agc_mode (device, 1);

	running. store (true);
	return true;
}

void	rtlsdrHandler::stopReader		(void) {
	if (!running. load ())
	   return;

	rtlsdr_cancel_async (device);
	workerHandle. join ();
	running. store (false);
}
//
//	we only have 8 bits, so rather than doing a float division to get
//	the float value we want, we precompute the possibilities
static 
float convTable [] = {
 -128 / 128.0 , -127 / 128.0 , -126 / 128.0 , -125 / 128.0 , -124 / 128.0 , -123 / 128.0 , -122 / 128.0 , -121 / 128.0 , -120 / 128.0 , -119 / 128.0 , -118 / 128.0 , -117 / 128.0 , -116 / 128.0 , -115 / 128.0 , -114 / 128.0 , -113 / 128.0 
, -112 / 128.0 , -111 / 128.0 , -110 / 128.0 , -109 / 128.0 , -108 / 128.0 , -107 / 128.0 , -106 / 128.0 , -105 / 128.0 , -104 / 128.0 , -103 / 128.0 , -102 / 128.0 , -101 / 128.0 , -100 / 128.0 , -99 / 128.0 , -98 / 128.0 , -97 / 128.0 
, -96 / 128.0 , -95 / 128.0 , -94 / 128.0 , -93 / 128.0 , -92 / 128.0 , -91 / 128.0 , -90 / 128.0 , -89 / 128.0 , -88 / 128.0 , -87 / 128.0 , -86 / 128.0 , -85 / 128.0 , -84 / 128.0 , -83 / 128.0 , -82 / 128.0 , -81 / 128.0 
, -80 / 128.0 , -79 / 128.0 , -78 / 128.0 , -77 / 128.0 , -76 / 128.0 , -75 / 128.0 , -74 / 128.0 , -73 / 128.0 , -72 / 128.0 , -71 / 128.0 , -70 / 128.0 , -69 / 128.0 , -68 / 128.0 , -67 / 128.0 , -66 / 128.0 , -65 / 128.0 
, -64 / 128.0 , -63 / 128.0 , -62 / 128.0 , -61 / 128.0 , -60 / 128.0 , -59 / 128.0 , -58 / 128.0 , -57 / 128.0 , -56 / 128.0 , -55 / 128.0 , -54 / 128.0 , -53 / 128.0 , -52 / 128.0 , -51 / 128.0 , -50 / 128.0 , -49 / 128.0 
, -48 / 128.0 , -47 / 128.0 , -46 / 128.0 , -45 / 128.0 , -44 / 128.0 , -43 / 128.0 , -42 / 128.0 , -41 / 128.0 , -40 / 128.0 , -39 / 128.0 , -38 / 128.0 , -37 / 128.0 , -36 / 128.0 , -35 / 128.0 , -34 / 128.0 , -33 / 128.0 
, -32 / 128.0 , -31 / 128.0 , -30 / 128.0 , -29 / 128.0 , -28 / 128.0 , -27 / 128.0 , -26 / 128.0 , -25 / 128.0 , -24 / 128.0 , -23 / 128.0 , -22 / 128.0 , -21 / 128.0 , -20 / 128.0 , -19 / 128.0 , -18 / 128.0 , -17 / 128.0 
, -16 / 128.0 , -15 / 128.0 , -14 / 128.0 , -13 / 128.0 , -12 / 128.0 , -11 / 128.0 , -10 / 128.0 , -9 / 128.0 , -8 / 128.0 , -7 / 128.0 , -6 / 128.0 , -5 / 128.0 , -4 / 128.0 , -3 / 128.0 , -2 / 128.0 , -1 / 128.0 
, 0 / 128.0 , 1 / 128.0 , 2 / 128.0 , 3 / 128.0 , 4 / 128.0 , 5 / 128.0 , 6 / 128.0 , 7 / 128.0 , 8 / 128.0 , 9 / 128.0 , 10 / 128.0 , 11 / 128.0 , 12 / 128.0 , 13 / 128.0 , 14 / 128.0 , 15 / 128.0 
, 16 / 128.0 , 17 / 128.0 , 18 / 128.0 , 19 / 128.0 , 20 / 128.0 , 21 / 128.0 , 22 / 128.0 , 23 / 128.0 , 24 / 128.0 , 25 / 128.0 , 26 / 128.0 , 27 / 128.0 , 28 / 128.0 , 29 / 128.0 , 30 / 128.0 , 31 / 128.0 
, 32 / 128.0 , 33 / 128.0 , 34 / 128.0 , 35 / 128.0 , 36 / 128.0 , 37 / 128.0 , 38 / 128.0 , 39 / 128.0 , 40 / 128.0 , 41 / 128.0 , 42 / 128.0 , 43 / 128.0 , 44 / 128.0 , 45 / 128.0 , 46 / 128.0 , 47 / 128.0 
, 48 / 128.0 , 49 / 128.0 , 50 / 128.0 , 51 / 128.0 , 52 / 128.0 , 53 / 128.0 , 54 / 128.0 , 55 / 128.0 , 56 / 128.0 , 57 / 128.0 , 58 / 128.0 , 59 / 128.0 , 60 / 128.0 , 61 / 128.0 , 62 / 128.0 , 63 / 128.0 
, 64 / 128.0 , 65 / 128.0 , 66 / 128.0 , 67 / 128.0 , 68 / 128.0 , 69 / 128.0 , 70 / 128.0 , 71 / 128.0 , 72 / 128.0 , 73 / 128.0 , 74 / 128.0 , 75 / 128.0 , 76 / 128.0 , 77 / 128.0 , 78 / 128.0 , 79 / 128.0 
, 80 / 128.0 , 81 / 128.0 , 82 / 128.0 , 83 / 128.0 , 84 / 128.0 , 85 / 128.0 , 86 / 128.0 , 87 / 128.0 , 88 / 128.0 , 89 / 128.0 , 90 / 128.0 , 91 / 128.0 , 92 / 128.0 , 93 / 128.0 , 94 / 128.0 , 95 / 128.0 
, 96 / 128.0 , 97 / 128.0 , 98 / 128.0 , 99 / 128.0 , 100 / 128.0 , 101 / 128.0 , 102 / 128.0 , 103 / 128.0 , 104 / 128.0 , 105 / 128.0 , 106 / 128.0 , 107 / 128.0 , 108 / 128.0 , 109 / 128.0 , 110 / 128.0 , 111 / 128.0 
, 112 / 128.0 , 113 / 128.0 , 114 / 128.0 , 115 / 128.0 , 116 / 128.0 , 117 / 128.0 , 118 / 128.0 , 119 / 128.0 , 120 / 128.0 , 121 / 128.0 , 122 / 128.0 , 123 / 128.0 , 124 / 128.0 , 125 / 128.0 , 126 / 128.0 , 127 / 128.0 };

//
//	The brave old getSamples. For the dab stick, we get
//	size samples: still in I/Q pairs, but we have to convert the data from
//	uint8_t to std::complex<float> *
int32_t	rtlsdrHandler::getSamples (std::complex<float> *V, int32_t size) { 
int32_t	amount, i;
uint8_t	tempBuffer [2 * size];
//
	amount = _I_Buffer	-> getDataFromBuffer (tempBuffer, 2 * size);
	for (i = 0; i < amount / 2; i ++)
            V [i] = std::complex<float>
                            (convTable [tempBuffer [2 * i]],
                             convTable [tempBuffer [2 * i + 1]]);
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

