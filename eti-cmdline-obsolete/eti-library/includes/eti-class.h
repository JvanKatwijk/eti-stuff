#
/*
 *    Copyright (C)  2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the DAB-cmdline program.
 *    Many of the ideas as implemented in DAB-cmdline are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    DAB-cmdline is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    DAB-cmdline is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with DAB-cmdline; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __ETI_CLASS__
#define __ETI_CLASS__

#include	"dab-constants.h"
#include	"eti-api.h"
#include	<stdio.h>
#include	<vector>
#include	<string>
#include	<list>
#include	<atomic>
#include	<thread>

#include	"ofdm-processor.h"
#include	"eti-generator.h"
#include	"ringbuffer.h"
#include	"ensemble-handler.h"

#include	<mutex>
#include	<condition_variable>

class	deviceHandler;

class etiClass {
public:
        		etiClass	(deviceHandler	*,
	                                 uint8_t,
	                         	 dabBand,
	                                 int16_t,		// waiting time
	                                 FILE *);
			~etiClass	(void);
	void		eti_gain	(uint16_t);
	bool		eti_running	(void);
	bool		eti_channel	(std::string s);
	void		eti_run		(cb_ensemble_t);
	void		eti_stop	(void);
	void		eti_go		(void);
	bool		ensembleArrived	(void);
private:
	deviceHandler	*inputDevice;
	uint8_t		theMode;
	dabBand		theBand;
	int16_t		waitingTime;
	std::thread     threadHandle;
	FILE		*etiFile;
	void		run_dab		(cb_ensemble_t);
	std::atomic<bool> run;
	ofdmProcessor	*my_ofdmProcessor;
	int16_t		deviceGain;
	std::string	requestedChannel;
	ensembleHandler theEnsemble;
	etiGenerator	*my_etiGenerator;
};

#endif
