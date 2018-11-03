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

#ifdef	__MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

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
	(theStick -> rtlsdr_read_async) (theStick -> device,
	                          (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                          (void *)theStick,
	                          0,
	                          READLEN_DEFAULT);
}
//
//	Our wrapper is a simple classs
	rtlsdrHandler::rtlsdrHandler (int frequency, int16_t ppm,
	                              int16_t gain, bool autogain) {
int16_t	deviceCount;
int32_t	r;
int16_t	deviceIndex;

	this	-> frequency     = frequency;
	this	-> ppmCorrection = ppm;
	this	-> theGain       = gain;
	this	-> autogain      = autogain;

	inputRate		= 2048000;
	libraryLoaded		= false;
	open			= false;
	_I_Buffer		= NULL;
	this	-> sampleCounter= 0;
	this	-> vfoOffset	= 0;
	gains			= NULL;
	running			= false;

#ifdef	__MINGW32__
	const char *libraryString = "rtlsdr.dll";
	Handle		= LoadLibrary ((wchar_t *)L"rtlsdr.dll");
#else
	const char *libraryString = "librtlsdr.so";
	Handle		= dlopen ("librtlsdr.so", RTLD_NOW);
#endif

	if (Handle == NULL) {
	   fprintf (stderr, "failed to open %s\n", libraryString);
	   throw (21);
	}

	libraryLoaded	= true;
	if (!load_rtlFunctions ()) 
	   goto err;
//
//	Ok, from here we have the library functions accessible
	deviceCount 		= this -> rtlsdr_get_device_count ();
	if (deviceCount == 0) {
	   fprintf (stderr, "No devices found\n");
	   goto err;
	}

	deviceIndex = 0;	// default
//
//	OK, now open the hardware
	r			= this -> rtlsdr_open (&device, deviceIndex);
	if (r < 0) {
	   fprintf (stderr, "Opening dabstick failed\n");
	   goto err;
	}

	open			= true;
	r			= this -> rtlsdr_set_sample_rate (device,
	                                                          inputRate);
	if (r < 0) {
	   fprintf (stderr, "Setting samplerate failed\n");
	   goto err;
	}

	r		= this -> rtlsdr_get_sample_rate (device);
	fprintf (stderr, "samplerate set to %d\n", r);
	rtlsdr_set_tuner_gain_mode (device, 0);

	gainsCount	= rtlsdr_get_tuner_gains (device, NULL);
	fprintf (stderr, "Supported gain values: %d\n", gainsCount);
	gains		= new int [gainsCount];
	gainsCount	= rtlsdr_get_tuner_gains (device, gains);
        rtlsdr_set_tuner_gain_mode (device, autogain);
	rtlsdr_set_tuner_gain (device,gains [theGain * gainsCount / 100]);
        
	r		= rtlsdr_set_freq_correction (device, ppmCorrection);

        if (r == 0) {
	   r = rtlsdr_get_freq_correction(device);
	   fprintf (stderr, "Frequency correction set to: %d ppm\n", r);
	}
	else {
	   fprintf (stderr, "Setting frequency correction failed\n");
	}

	_I_Buffer	= new RingBuffer<uint8_t>(1024 * 1024);
	return;

err:
	if (open)
	   this -> rtlsdr_close (device);
#ifdef __MINGW32__
	FreeLibrary (Handle);
#else
	dlclose (Handle);
#endif
	libraryLoaded	= false;
	open		= false;
	throw (33);
}

	rtlsdrHandler::~rtlsdrHandler	(void) {
	if (running) { // we are running
	   this -> rtlsdr_cancel_async (device);
	   workerHandle. join ();
	}
	running	= false;
	if (open)
	   this -> rtlsdr_close (device);
	if (Handle != NULL) 
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
	if (_I_Buffer != NULL)
	   delete _I_Buffer;
	if (gains != NULL)
	   delete[] gains;
	open = false;
}

void	rtlsdrHandler::setVFOFrequency	(int32_t f) {
	frequency	= f;
	(void)(this -> rtlsdr_set_center_freq (device, f + vfoOffset));
}

int32_t	rtlsdrHandler::getVFOFrequency	(void) {
	return (int32_t)(this -> rtlsdr_get_center_freq (device)) - vfoOffset;
}
//
//
bool	rtlsdrHandler::restartReader	(void) {
int32_t	r;

	if (running)
	   return true;

	_I_Buffer	-> FlushRingBuffer ();
	r = this -> rtlsdr_reset_buffer (device);
	if (r < 0)
	   return false;

	this -> rtlsdr_set_center_freq (device, frequency + vfoOffset);
	workerHandle = std::thread (controlThread, this);
	rtlsdr_set_tuner_gain_mode (device, autogain);
	rtlsdr_set_tuner_gain (device, gains [theGain * gainsCount / 100]);
	running	= true;
	return true;
}

void	rtlsdrHandler::stopReader		(void) {
	if (!running)
	   return;

	this -> rtlsdr_cancel_async (device);
	workerHandle. join ();
	running	= false;
}
//
//	when selecting with an integer in the range 0 .. 100
//	first find the table value
void	rtlsdrHandler::setGain	(int32_t g) {
	theGain	= g;
	rtlsdr_set_tuner_gain (device, gains [theGain * gainsCount / 100]);
}
	
void	rtlsdrHandler::setAgc		(bool b) {
	rtlsdr_set_tuner_gain_mode (device, b);
	rtlsdr_set_tuner_gain (device, gains [theGain * gainsCount / 100]);
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
//
bool	rtlsdrHandler::load_rtlFunctions (void) {
//
//	link the required procedures
	rtlsdr_open	= (pfnrtlsdr_open)
	                       GETPROCADDRESS (Handle, "rtlsdr_open");
	if (rtlsdr_open == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_open\n");
	   return false;
	}
	rtlsdr_close	= (pfnrtlsdr_close)
	                     GETPROCADDRESS (Handle, "rtlsdr_close");
	if (rtlsdr_close == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_close\n");
	   return false;
	}

	rtlsdr_set_sample_rate =
	    (pfnrtlsdr_set_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_set_sample_rate");
	if (rtlsdr_set_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_sample_rate\n");
	   return false;
	}

	rtlsdr_get_sample_rate	=
	    (pfnrtlsdr_get_sample_rate)GETPROCADDRESS (Handle, "rtlsdr_get_sample_rate");
	if (rtlsdr_get_sample_rate == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_sample_rate\n");
	   return false;
	}

	rtlsdr_get_tuner_gains		= (pfnrtlsdr_get_tuner_gains)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gains");
	if (rtlsdr_get_tuner_gains == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gains\n");
	   return false;
	}


	rtlsdr_set_tuner_gain_mode	= (pfnrtlsdr_set_tuner_gain_mode)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain_mode");
	if (rtlsdr_set_tuner_gain_mode == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_tuner_gain_mode\n");
	   return false;
	}

	rtlsdr_set_tuner_gain	= (pfnrtlsdr_set_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_tuner_gain");
	if (rtlsdr_set_tuner_gain == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_set_tuner_gain\n");
	   return false;
	}

	rtlsdr_get_tuner_gain	= (pfnrtlsdr_get_tuner_gain)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_tuner_gain");
	if (rtlsdr_get_tuner_gain == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_tuner_gain\n");
	   return false;
	}
	rtlsdr_set_center_freq	= (pfnrtlsdr_set_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_set_center_freq");
	if (rtlsdr_set_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_center_freq\n");
	   return false;
	}

	rtlsdr_get_center_freq	= (pfnrtlsdr_get_center_freq)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_center_freq");
	if (rtlsdr_get_center_freq == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_center_freq\n");
	   return false;
	}

	rtlsdr_reset_buffer	= (pfnrtlsdr_reset_buffer)
	                     GETPROCADDRESS (Handle, "rtlsdr_reset_buffer");
	if (rtlsdr_reset_buffer == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_reset_buffer\n");
	   return false;
	}

	rtlsdr_read_async	= (pfnrtlsdr_read_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_read_async");
	if (rtlsdr_read_async == NULL) {
	   fprintf (stderr, "Cound not find rtlsdr_read_async\n");
	   return false;
	}

	rtlsdr_get_device_count	= (pfnrtlsdr_get_device_count)
	                     GETPROCADDRESS (Handle, "rtlsdr_get_device_count");
	if (rtlsdr_get_device_count == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_count\n");
	   return false;
	}

	rtlsdr_cancel_async	= (pfnrtlsdr_cancel_async)
	                     GETPROCADDRESS (Handle, "rtlsdr_cancel_async");
	if (rtlsdr_cancel_async == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_cancel_async\n");
	   return false;
	}

	rtlsdr_set_direct_sampling = (pfnrtlsdr_set_direct_sampling)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_direct_sampling");
	if (rtlsdr_set_direct_sampling == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_direct_sampling\n");
	   return false;
	}

	rtlsdr_set_freq_correction = (pfnrtlsdr_set_freq_correction)
	                  GETPROCADDRESS (Handle, "rtlsdr_set_freq_correction");
	if (rtlsdr_set_freq_correction == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_set_freq_correction\n");
	   return false;
	}
	
        rtlsdr_get_freq_correction = (pfnrtlsdr_get_freq_correction)
	                  GETPROCADDRESS (Handle, "rtlsdr_get_freq_correction");
	if (rtlsdr_get_freq_correction == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_freq_correction\n");
	   return false;
	}

	rtlsdr_get_device_name = (pfnrtlsdr_get_device_name)
	                  GETPROCADDRESS (Handle, "rtlsdr_get_device_name");
	if (rtlsdr_get_device_name == NULL) {
	   fprintf (stderr, "Could not find rtlsdr_get_device_name\n");
	   return false;
	}

	fprintf (stderr, "OK, functions seem to be loaded\n");
	return true;
}

void	rtlsdrHandler::resetBuffer (void) {
	_I_Buffer -> FlushRingBuffer ();
}

int16_t	rtlsdrHandler::maxGain	(void) {
	return gainsCount;
}

int16_t	rtlsdrHandler::bitDepth	(void) {
	return 8;
}

