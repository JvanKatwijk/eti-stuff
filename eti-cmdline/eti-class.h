#
/*
 *    Copyright (C) 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti-cmdline program.
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
#
#ifndef	__ETI_CLASS__
#define	__ETI_CLASS__
#include	<string>
#include	<stdint.h>
#include	<stdio.h>
#include	"callback-types.h"
#include	"ofdm-processor.h"
#include	"eti-generator.h"
#include	"device-handler.h"
#ifdef	HAVE_DUMPING
#include	"sndfile.h"
#endif
//
//
//	A convenience class for the use of the functionality of the
//	dab -> eti library.
//	The class combined the functionality of the ofdmProcessor class
//	and the etiGenerator class
//	The ofdmProcessor class does - as the name suggests - the 
//	ofdm handling, the raw decoded ofdm blocks are passed on to
//	the etiGenerator class. The latter does the interpretation
//	and the translation to eti format.
//
//	Communication with the "outside" world is through some callbacks
//	a. the syncsignal_t syncsignalHandler is called (once) to
//	indicate whether or not the software could time synchronize
//	in which case it is assumed there is a DAB signal
//	b. the snrsignal_t snrsignalHandler  is called app once a second
//	it tells a rought SNR.
//	c. the ensemblename_t ensembleHandler is called whenever the
//	name of an ensemble is detected. The general idea is that when
//	such a name is detected, the frequency synchronization is OK
//	and the process of update frequency synchronization can be stopped.
//	The function migh be called several times.
//	d. the programname_t programNameHandler is called whenever the
//	name of a program in the ensemble is detected.
//	e. the fibquality_t fibqualityHandler tells the percentage of
//	fib blocks that successfully passed the crc test.
//	the etiwriter_t	etiwriterHandler is called whever the eti processing
//	is on, it just hands over the eti frames. The current setup is unbuffered.

class	etiClass {
public:
//
//	Initialization:
		etiClass	       (uint8_t	Mode,
	                                deviceHandler	*handler,
#ifdef	HAVE_DUMPING
	                                SNDFILE		*dumpFile,
#endif
//	The callbacks
		  			syncsignal_t	syncsignalHandler,
	          	                snrsignal_t	snrsignalHandler,
		  	                ensemblename_t	ensembleHandler,
	          	                programname_t	programNameHandler,
	          	                fibquality_t	fibqualityHandler,
	          	                etiwriter_t	etiwriterHandler,
//	context or other user data
	                                void		*userData
	         	               );

		~etiClass		(void);
//
//	the ofdm processing, i.e. the thread executing the code that does
//	the work, is started here
	void	start_ofdmProcessing	(void);
//
//	the eti generation will start from here, note that on creating
//	an instance of the class etiprocessor, the thread executing the
//	code that interprets data runs
	void	start_etiProcessing	(void);
//
//	stop will definitely stop, no restart is possible
	void	stop			(void);
//
//	reset will stop all processing, restart possible
	void	reset			(void);
//
//	set_syncReached is a signal to the ofdmprocessor that 
//	frequency synchronization is OK
	void	set_syncReached		(void);
//
//	with Working the caller can see whether the ofdmprocessor
//	is running
	bool	Working			(void);
private:
	ofdmProcessor	my_ofdmProcessor;
	etiGenerator	my_etiGenerator;
};

#endif
