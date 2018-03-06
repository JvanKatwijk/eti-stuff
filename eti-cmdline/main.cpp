#
/*
 *    Copyright (C) 2016, 2017, 2018
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
 *
 *	This is an example main program, just to show how the software
 *	is to be used. Feel free to adapt to your needs
 */
#include	<unistd.h>
#include	<signal.h>
#include	<getopt.h>
#include	<atomic>
#include        <cstdio>
#include        <iostream>
#include	<mutex>
#include	"device-handler.h"
#include	"band-handler.h"
#include	"eti-class.h"
#ifdef	HAVE_DUMPING
#include	"sndfile.h"
#endif

using std::cerr;
using std::endl;
//
#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler.h"
#elif HAVE_SDRPLAY
#include	"sdrplay-handler.h"
#elif HAVE_AIRSPY
#include	"airspy-handler.h"
#elif	HAVE_RAWFILES
#include	"rawfile-handler.h"
#elif	HAVE_WAVFILES
#include	"wavfile-handler.h"
#elif	HAVE_RTL_TCP
#include	"rtl_tcp-client.h"
#endif
//
//	Be aware that the callbacks may arrive from different threads,
//	and most likely do not know from each other. So, apart from
//	explicitly declaring some variables as being "atomic", 
//	a light form of locking might be required.
static
std::atomic<bool> run;
static
std::atomic<bool>timeSynced;
static
std::atomic<bool>timesyncSet;
static
std::atomic<int> signalnoise;
static
std::atomic<int16_t> ficSuccess;
static
std::atomic<bool>ensembleRecognized;

static
bool	isSilent	= false;

std::string	theName;
std::mutex	mainLocker;

static void sighandler (int signum) {
        fprintf (stderr, "\n\nSignal %d caught, terminating!\n", signum);
	run. store (false);
}
//
//
//	The signal from the ofdmprocessor about believing that there
//	is a DAB signal in the datastream just sets some variables here.
//	These variables are read in other parts of the program
void	syncsignalHandler (bool b, void *ctx) {
	(void)ctx;
	timesyncSet. store (true);	// not needed actually
	timeSynced. store (b);
}
//
//	The snrsignalHandler is called on a regular basis by the
//	ofdmProcessor. It merely sets a variable here, other parts
//	of the program will show or interpret the value
void	snrsignalHandler (int16_t snr, void *ctx) {
	(void)ctx;
	signalnoise. store (snr);
}
//
//	The same for the fibqualityHandler
void	fibqualityHandler (int16_t success, void *ctx) {
	(void)ctx;
	ficSuccess. store (success);
}
//
//	the function programnameHandler is called whever the
//	name of a program in the ensemble is detected. It is called
//	once for each programname
static
int	programCounter	= 1;
void	programnameHandler (std::string name, int SId, void *ctx) {
	(void)ctx;
	mainLocker. lock ();
	fprintf (stderr, "program\ (%2d)\t %s\t \%lX is in the list\n",
	                               programCounter ++, name. c_str (), SId);
	mainLocker. unlock ();
}
//
//	the function ensembleHandler is called when the name of the
//	ensemble is detected. The function may be called several times
//	(all occurrences with the same name)
void	ensemblenameHandler (std::string ensemblename, void *ctx) {
	(void)ctx;
	if (ensembleRecognized. load ())
	   return;
	mainLocker. lock ();
	ensembleRecognized. store (true);
	fprintf (stderr, "ensemble %s detected\n", ensemblename. c_str ());
	theName = ensemblename;
	mainLocker. unlock ();
}

//
//	The function etiwriteHandler is called each time an eti frame
//	is completed. Parameters are the frame and the number of bytes
static
FILE	*etiFile;
static
int	cnt	= 0;
void	etiwriterHandler (uint8_t *buffer, int32_t amount, void *ctx) {
	(void)ctx;
	fwrite (buffer, 1, amount, etiFile);
	if (!isSilent)
	   fprintf (stderr, "%d\r", ++cnt);
}

void	inputStopped	(void) {
	run. store (false);
}


void    printOptions (void);
//
int	main (int argc, char **argv) {
// Default values
int16_t		timeSyncTime	= 5;
int16_t		freqSyncTime	= 10;
uint8_t		theMode		= 1;
std::string	theChannel	= "11C";
uint8_t		theBand		= BAND_III;
int16_t		deviceGain	= 80;	// scale = 0 .. 100
bool		autoGain	= false;
int16_t		ppmCorrection	= 0;
deviceHandler	*inputDevice;
bandHandler	the_bandHandler;
int32_t		tunedFrequency	= 220000000;	// just a setting
#ifdef	HAVE_DUMPING
SNDFILE		*dumpFile	= NULL;
#endif
int	opt;
struct sigaction sigact;
#if defined(HAVE_RAWFILES) || defined(HAVE_WAVFILES)
std::string	inputfileName;
bool	continue_on_eof	= false;
#elif defined (HAVE_RTL_TCP)
std::string	hostname = "127.0.0.1";
int32_t		basePort = 1234;
#endif
//
//	default
	etiFile		= stdout;
	std::setlocale (LC_ALL, "");
	timeSynced.		store (false);
	timesyncSet.		store (false);
	ensembleRecognized.	store (false);
	run.			store (false);
//
//	for file input some command line parameters are meeaningless
#if defined (HAVE_RAWFILES) || defined (HAVE_WAVFILES)
	while ((opt = getopt (argc, argv, "ED:d:M:O:F:Sh")) != -1) {
#elif defined (HAVE_RTL_TCP)
	while ((opt = getopt (argc, argv, "D:d:M:B:C:G:O:P:H:I:QR:Sh")) != -1) {
#else
	while ((opt = getopt (argc, argv, "D:d:M:B:C:G:O:P:QR:Sh")) != -1) {
#endif
	   switch (opt) {
	      case 'D':
	         freqSyncTime	= atoi (optarg);
	         break;

	      case 'd':
	         timeSyncTime	= atoi (optarg);
	         break;

	      case 'O':
	         if (optarg [0] == '-') {
	            etiFile = stdout;
	            break;
	         }
	       
	         etiFile	= fopen (optarg, "w+b");
	         if (etiFile == NULL) {
	            fprintf (stderr, "eti file %s cannot be opened\n", optarg);
	            exit (23);
	         }

	         fprintf (stderr, "outputfile = %s\n", optarg);
	         break;

	      case 'R': 
#ifdef	HAVE_DUMPING
	         {  SF_INFO *sf_info  = (SF_INFO *)alloca (sizeof (SF_INFO));
	            sf_info -> samplerate   = INPUT_RATE;
	            sf_info -> channels     = 2;
	            sf_info -> format       = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	            dumpFile	= sf_open (optarg, SFM_WRITE, sf_info);
	            if (dumpFile == NULL) {
	               fprintf (stderr,
	                        "dumpfile %s cannot be opened\n", optarg);
	               exit (24);
	            }
	         }
#else
	         fprintf (stderr, "Option only effective for dump in config\n");
#endif
	         break;

	      case 'M':
	         theMode	= atoi (optarg);
	         if (!(theMode == 1) || (theMode == 2) || (theMode == 4))
	            theMode = 1; 
	         break;

#if	defined (HAVE_RAWFILES) || defined (HAVE_WAVFILES)
	      case 'F':
	         inputfileName	= std::string (optarg);
	         break;

	      case 'E':
	         continue_on_eof	= true;
	         fprintf (stderr, "continue_on_eof is now set\n");
	         break;
#else
#ifdef	HAVE_RTL_TCP
	      case 'H':
	         hostname	= std::string (optarg);
	         break;

	      case 'I':
	         basePort	= atoi (optarg);
	         break;
#endif
	      case 'B':
	         theBand = std::string (optarg) == std::string ("L_BAND") ?
	                                     L_BAND : BAND_III;
	         break;

	      case 'C':
	         theChannel	= std::string (optarg);
	         break;

	      case 'G':
	         deviceGain	= atoi (optarg);
	         break;

	      case 'Q':
	         autoGain	= true;
	         break;

	      case 'P':
	         ppmCorrection	= atoi (optarg);
	         break;
#endif
	      case 'S':
	         isSilent	= true;
	         break;

	      case 'h':
	         printOptions ();
	         exit (2);

	      default:
	         printOptions ();
	         exit (1);
	   }
	}

#if defined(HAVE_RAWFILES) || defined(HAVE_WAVFILES)
//	check on name ??
#else
	tunedFrequency	=  the_bandHandler. Frequency (theBand, theChannel);
	if (tunedFrequency == -1) {
	   fprintf (stderr, "Cannot handle channel %s\n", theChannel. c_str ());
	   exit (4);
	}
	cerr << "tunedFrequency =  " << tunedFrequency << endl;
#endif
//	The input device handler will generate an exception when
//	installation does not work

	try {
#ifdef	HAVE_RTLSDR
	   inputDevice	= new rtlsdrHandler (tunedFrequency,
	                                        deviceGain, autoGain);
#elif	HAVE_SDRPLAY
	   inputDevice	= new sdrplayHandler (tunedFrequency, ppmCorrection,
	                                        deviceGain, autoGain, 0, 0);
#elif	HAVE_AIRSPY
	   inputDevice	= new airspyHandler (tunedFrequency,
	                                        deviceGain, autoGain);
#elif	HAVE_RAWFILES
	   inputDevice	= new rawfileHandler (inputfileName,
	                                      continue_on_eof, 
	                                      inputStopped);
#elif	HAVE_WAVFILES
	   inputDevice	= new wavfileHandler (inputfileName,
	                                      continue_on_eof,
	                                      inputStopped);
#elif	HAVE_RTL_TCP
	   inputDevice = new rtl_tcp_client (hostname,
	                                     basePort,
	                                     tunedFrequency,
	                                     deviceGain,
	                                     autoGain,
	                                     ppmCorrection);
#endif
	}
	catch (int e) {
	   cerr << "Installing device failed, fatal" << e << endl;
	   exit (3);
	}
//
//	OK, it seems we have a device
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
//	sigaction(SIGTERM, &sigact, NULL);
//	sigaction(SIGQUIT, &sigact, NULL);

//
	etiClass theWorker (theMode,
	                    inputDevice,
#ifdef	HAVE_DUMPING
	                    dumpFile,
#endif
	                    &syncsignalHandler,
	                    &snrsignalHandler,
	                    &ensemblenameHandler,
	                    &programnameHandler,
	                    &fibqualityHandler,
	                    &etiwriterHandler,
	                    NULL);
	inputDevice	-> restartReader ();
	timesyncSet. store (false);
	theWorker. start_ofdmProcessing ();

	while (!timeSynced. load () && (--timeSyncTime >= 0)) 
//	while (!timesyncSet. load () && (--timeSyncTime >= 0)) 
	   sleep (1);

	if (!timeSynced. load ()) {
	   cerr << "There does not seem to be a DAB signal here" << endl;
	   exit (1);
	}
	else
	   cerr << "there might be a DAB signal here" << endl;

//	give time to collect data on the ensemble and the programs
//
	while (--freqSyncTime > 0) {
	   cerr << "still at most " << freqSyncTime <<  "seconds to wait\r";
	   sleep (1);
	   if (ensembleRecognized. load ()) {
	      theWorker. set_syncReached ();
	      break;
	   }
	}

	cerr << endl;

	if (!ensembleRecognized. load ()) {
	   cerr << "Further waiting does not seem useful, quitting" << endl;
	   theWorker. stop ();
	   exit (2);
	}
	else {
	   theWorker. start_etiProcessing ();
	   cerr << "Handling ensemble " << theName. c_str () <<
	                    "until you quit" << endl;
	   run. store (true);
	   while (run. load ()) {
	      if (!isSilent)
	         fprintf (stderr, "\t\testimated snr: %2d, fibquality %3d\r",
	                            signalnoise. load (), ficSuccess. load ());
	      sleep (1);
	   }
	}

//	we started the "worker", so we also stop it here
	theWorker. stop ();
	fprintf (stderr, "\n\nterminating\n");
	usleep (1000);

	inputDevice	-> stopReader ();
	delete	inputDevice;
	if (etiFile != NULL)
	   fclose (etiFile);
#ifdef	HAVE_DUMPING
	if (dumpFile != NULL)
	   sf_close (dumpFile);
#endif
	exit (1);
}

void    printOptions (void) {
        std::cerr << 
" eti-cmdline-xxx options are\n\
\n\
   -D number   time (in seconds) to look for a DAB ensemble\n\
   -B Band     select DAB Band (default: BAND_III, or L_BAND)\n"
#if defined (HAVE_RAWFILES) || defined (HAVE_WAVFILES)
<< "\
   -F filename load samples from file\n\
   -E          only for files: continue after EOF (replay file)\n"
#else
#if defined (HAVE_RTL_TCP)
<< "\
   -H hostname address of the RTL_TCP server\n\
   -I port     TCP listening port of the server\n"
#endif
<< "\
   -C channel  DAB channel to be used (5A ... 13F resp. LA ... LP)\n\
   -G Gain     gain for device (range 1 .. 100)\n\
   -P number   PPM correction\n\
   -Q          autogain for device (not all tuners support it!)\n\
   -R filename (if configured) dump to an *.sdr file\n"
#endif
#ifdef HAVE_DUMPING
<< "\
   -O filename write output into a file (instead of stdout)\n"
#endif
<< "\
   -S          do not display quality messages while running\n\
\n\
   -h          show options and quit\n";

}

