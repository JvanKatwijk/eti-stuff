#
/*
 *    Copyright (C)  2016 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is the implementation part of the eti api, it
 *    binds the dab-library to the api.
 *
 *    DAB-library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DAB-library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DAB-library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include	"eti-api.h"
#include	"eti-class.h"
#include	"dab-constants.h"
#include	<stdio.h>
#include	<stdint.h>
#include	"device-handler.h"
#ifdef	HAVE_DABSTICK
#include	"rtlsdr-handler.h"
#endif
#ifdef	HAVE_SDRPLAY
#include	"sdrplay-handler.h"
#endif
#ifdef	HAVE_AIRSPY
#include	"airspy-handler.h"
#endif

//
//	a global variable and some forward declarations

deviceHandler	*setDevice (void);

void	*eti_initialize	(uint8_t	dabMode,  // dab Mode
	                 dabBand	band,	  // Band
	                 int16_t	waitingTime,
	                 FILE		*etiFile
	                 ) {
deviceHandler *inputDevice;
	inputDevice	= setDevice ();
	if (inputDevice == NULL) {
	   fprintf (stderr, "Fatal error with device\n");
	   return NULL;
	}
	return (void *)(new etiClass (inputDevice,
	                              dabMode,
	                              band,
	                              waitingTime,
	                              etiFile));
}
	   
void	eti_Gain	(void *handle, uint16_t g) {
	if (((etiClass *)handle) != NULL)
	   ((etiClass *)handle) -> eti_gain (g);
}

bool	eti_Channel	(void *handle, std::string name) {
	if (((etiClass *)handle) == NULL)
	   return false;
	((etiClass *)handle) -> eti_stop ();
	return ((etiClass *)handle) -> eti_channel (name);
}

void	eti_run		(void *handle, cb_ensemble_t cb) {
	if (((etiClass *)handle) != NULL)
	   ((etiClass *)handle) -> eti_run (cb);
}

bool	is_running	(void *handle) {
	if (((etiClass *)handle) != NULL)
	   return ((etiClass *)handle) -> eti_running ();
	return false;
}

void	eti_go		(void *handle) {
        if (((etiClass *)handle) == NULL)
           return;
       if (!((etiClass *)handle) -> eti_running ())
           return;
        ((etiClass *)handle) -> eti_go ();
}

//
void	eti_stop	(void	*handle) {
	if (((etiClass *)handle) == NULL)
	   return;
	if (!((etiClass *)handle) -> eti_running ())
	   return;
	((etiClass *)handle) -> eti_stop ();
}
//
void	eti_exit	(void **handle_p) {
	if (((etiClass *)(*handle_p)) == NULL)
	   return;
	eti_stop (*handle_p);
	delete ((etiClass *)(*handle_p));
	*handle_p	= NULL;
}


deviceHandler	*setDevice (void) {
bool	success;
deviceHandler	*inputDevice;

#ifdef HAVE_AIRSPY
	inputDevice	= new airspyHandler (&success, 80, Mhz (220));
	fprintf (stderr, "device selected\n");
	if (!success) {
	   delete inputDevice;
	   return NULL;
	}
	else 
	   return inputDevice;
#elif defined (HAVE_SDRPLAY)
	inputDevice	= new sdrplayHandler (&success, 60, Mhz (220));
	if (!success) {
	   delete inputDevice;
	   return NULL;
	}
	else 
	   return inputDevice;
#elif defined (HAVE_DABSTICK)
	inputDevice	= new rtlsdrHandler (&success, 75, KHz (220000));
	if (!success) {
	   delete inputDevice;
	   return NULL;
	}
	else
	   return inputDevice;
#endif
	fprintf (stderr, "it appeared that you did not select a device, \nyou can run, but you will be connected to a virtual device, only providing zeros\n");
//	and as default option, we have a "no device"
	return  new deviceHandler ();
}

