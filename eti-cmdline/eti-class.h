#
/*
 *    Copyright (C) 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

class	etiClass {
public:
//
//	Initialization:
		etiClass	       (uint8_t	Mode,
	                                deviceHandler	*handler,
	                                int		nrProcessors,
#ifdef	HAVE_DUMPING
	                                SNDFILE		*dumpFile,
#endif
//	The callbacks
	                                callbacks	*the_callBacks,
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
	etiGenerator	my_etiGenerator;
	ofdmProcessor	my_ofdmProcessor;
};

#endif
