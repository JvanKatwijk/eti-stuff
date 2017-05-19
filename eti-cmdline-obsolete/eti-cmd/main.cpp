#
/*
 *    Copyright (C) 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti-cmdline program.
 *    Many of the ideas as implemented in DAB-cmdline are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *
 *	E X A M P L E  P R O G R A M
 *	for the ETI-library
 */
#include	<unistd.h>
#include	<signal.h>
#include	<getopt.h>
#include	"eti-api.h"
#include	<atomic>
//
//	we deal with a simple callback, so we have some data that needs
//	to be accessed from global contexts
static
std::atomic<bool> run;

static
void		*theRadio	= NULL;

static void sighandler (int signum) {
        fprintf (stderr, "Signal caught, terminating!\n");
	run. store (false);
}
//
//	This function is called whenever the dab engine has taken
//	some time to gather information from the FIC bloks
//	the Boolean b tells whether or not an ensemble has been
//	recognized, the names of the programs are in the 
//	ensemble
static
void	callblockHandler (std::list<std::string> ensemble, bool b) {
	if (!b) {
	   fprintf (stderr, "no ensemble, quitting");
	   run. store (false);
	   return;
	}
//	it seems we have an ensemble, so
	eti_go (theRadio);
}
	
int	main (int argc, char **argv) {
// Default values
int16_t		del		= 10;
uint8_t		theMode		= 1;
std::string	theChannel	= "11C";
dabBand		theBand		= BAND_III;
int		theGain		= 80;	// scale = 0 .. 100
int	opt;
struct sigaction sigact;
FILE	*etiFile = stdout;
//
	while ((opt = getopt (argc, argv, "D:M:B:C:G:F:")) != -1) {
	   switch (opt) {

	      case 'D':
	         del	= atoi (optarg);
	         break;

	      case 'M':
	         theMode	= atoi (optarg);
	         if (!(theMode == 1) || (theMode == 2) || (theMode == 4))
	            theMode = 1; 
	         break;

	      case 'B':
	         theBand = std::string (optarg) == std::string ("L_BAND") ?
	                                     L_BAND : BAND_III;
	         break;

	      case 'C':
	         theChannel	= std::string (optarg);
	         break;

	      case 'G':
	         theGain	= atoi (optarg);
	         break;

	      case 'F':
	         etiFile	= fopen (optarg, "w");
	         if (etiFile == NULL) {
	            fprintf (stderr, "%s cannot be opened\n", optarg);
	            exit (23);
	         }
	         break;

	      default:
	         break;
	   }
	}
//
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
//	sigaction(SIGINT, &sigact, NULL);
//	sigaction(SIGTERM, &sigact, NULL);
//	sigaction(SIGQUIT, &sigact, NULL);

	theRadio	= eti_initialize (theMode, theBand, 10, etiFile);
	if (theRadio == NULL) {
	   fprintf (stderr, "sorry, no radio available, fatal\n");
	   exit (4);
	}

	eti_Gain	(theRadio, theGain);
	eti_Channel	(theRadio, theChannel);
	eti_run		(theRadio, callblockHandler);
//
//	Note that while "eti_run" returns, another thread will be running
//	for controlling the dab decoding process.

	run. store (true);
	while (run. load () || is_running (theRadio))
	   sleep (1);
	eti_stop (theRadio);
	eti_exit (&theRadio);
}

