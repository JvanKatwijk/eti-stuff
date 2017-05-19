#
/*
 *    Copyright (C) 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  DAB-cmdline
 *    Many of the ideas as implemented in DAB-cmdline are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are acknowledged.
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
#include	<unistd.h>
#include	<termios.h>
#include	<stdio.h>
#include	<sys/select.h>
#include	<atomic>
#include	"dab-constants.h"
#include	"eti-class.h"
#include	"device-handler.h"


	etiClass::etiClass (deviceHandler	*inputDevice,
	                    uint8_t		dabMode,
	                    dabBand		theBand,
	                    int16_t		waitingTime,
	                    FILE		*etiFile) {
	this	-> inputDevice		= inputDevice;
	this	-> theMode		= dabMode;
	this	-> theBand		= theBand;
	this	-> waitingTime		= waitingTime;
	this	-> etiFile		= etiFile;

	deviceGain			= 50;
	theEnsemble. clearEnsemble ();
	my_etiGenerator	= new etiGenerator    (theMode,
	                                       &theEnsemble,
	                                       etiFile);
	my_ofdmProcessor = new ofdmProcessor   (inputDevice,
	                                        theMode,
	                                        my_etiGenerator,
	                                        3, 2);
	run. store (false);
}

	etiClass::~etiClass	(void) {
	if (run. load ()) {	// should not happen
	   run. store (false);
	   threadHandle. join ();
	}

	delete my_etiGenerator;
	delete my_ofdmProcessor;
}

void	etiClass::eti_gain	(uint16_t g) {
	deviceGain	= g;
	inputDevice	-> setGain (g);
}

bool	etiClass::eti_running	(void) {
	return run. load ();
}

struct dabFrequencies {
	const char	*key;
	int	fKHz;
};

struct dabFrequencies bandIII_frequencies [] = {
{"5A",	174928},
{"5B",	176640},
{"5C",	178352},
{"5D",	180064},
{"6A",	181936},
{"6B",	183648},
{"6C",	185360},
{"6D",	187072},
{"7A",	188928},
{"7B",	190640},
{"7C",	192352},
{"7D",	194064},
{"8A",	195936},
{"8B",	197648},
{"8C",	199360},
{"8D",	201072},
{"9A",	202928},
{"9B",	204640},
{"9C",	206352},
{"9D",	208064},
{"10A",	209936},
{"10B", 211648},
{"10C", 213360},
{"10D", 215072},
{"11A", 216928},
{"11B",	218640},
{"11C",	220352},
{"11D",	222064},
{"12A",	223936},
{"12B",	225648},
{"12C",	227360},
{"12D",	229072},
{"13A",	230748},
{"13B",	232496},
{"13C",	234208},
{"13D",	235776},
{"13E",	237488},
{"13F",	239200},
{NULL, 0}
};

struct dabFrequencies Lband_frequencies [] = {
{"LA", 1452960},
{"LB", 1454672},
{"LC", 1456384},
{"LD", 1458096},
{"LE", 1459808},
{"LF", 1461520},
{"LG", 1463232},
{"LH", 1464944},
{"LI", 1466656},
{"LJ", 1468368},
{"LK", 1470080},
{"LL", 1471792},
{"LM", 1473504},
{"LN", 1475216},
{"LO", 1476928},
{"LP", 1478640},
{NULL, 0}
};
/**
  */
bool	etiClass::eti_channel (std::string s) {
int16_t	i;
struct dabFrequencies *finger;
int32_t	tunedFrequency;

	tunedFrequency		= 0;
	if (theBand == BAND_III)
	   finger = bandIII_frequencies;
	else
	   finger = Lband_frequencies;

	for (i = 0; finger [i]. key != NULL; i ++) {
	   if (std::string (finger [i]. key) == s) {
	      tunedFrequency	= KHz (finger [i]. fKHz);
	      break;
	   }
	}

	if (tunedFrequency == 0) {
	   fprintf (stderr, "could not find a legal frequency for channel %s\n",
	                                     s. c_str ());
	   return false;
	}

	inputDevice		-> setVFOFrequency (tunedFrequency);
	return true;
}

static
cb_ensemble_t ff;
void	etiClass::eti_run	(cb_ensemble_t cb) {
	if (run. load ())
	   return;
	ff	= cb;
	threadHandle    = std::thread (&etiClass::run_dab, this, std::ref(ff));
}

void	etiClass::run_dab	(cb_ensemble_t h) {
int16_t	i;
	if (run. load ())
	   return;

//	Note: the ofdmProcessor will restart the inputDevice
	my_ofdmProcessor	-> start ();
	inputDevice		-> setGain (deviceGain);
	run. store (true);
// give everyone the opportunity to do domething
	for (i = 0; i < waitingTime; i ++) {
	   sleep (1);
	   if (theEnsemble. ensembleExists ())
	      my_ofdmProcessor -> syncReached ();
	}
	h (theEnsemble. data (), theEnsemble. ensembleExists ());
	if (!theEnsemble. ensembleExists ()) 
	   fprintf (stderr, "Further waiting does not make sense, quitting\n");	
	else
	   while (run. load ())
	      usleep (10000);
//
//	we started the ofdmprocessor, so we also stop it here
	my_ofdmProcessor	-> stop ();
	my_etiGenerator		-> stop ();
}

void	etiClass::eti_stop (void) {
	if (run. load ()) {
	   run. store (false);
	   threadHandle. join ();
	}
}

void	etiClass::eti_go (void) {
	if (run. load ()) {
	   my_etiGenerator	-> startProcessing ();
	}
}

bool	etiClass::ensembleArrived (void) {
	return theEnsemble. ensembleExists  ();
}

