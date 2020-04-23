#
/*
 *    Copyright (C) 2016, 2017, 2018
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
#elif	HAVE_SDRPLAY
#include	"sdrplay-handler.h"
#elif	HAVE_AIRSPY
#include	"airspy-handler.h"
#elif	HAVE_HACKRF
#include	"hackrf-handler.h"
#elif	HAVE_LIME
#include	"lime-handler.h"
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
	fprintf (stderr, "program\t (%2d)\t %s\t %X is in the list\n",
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
deviceHandler	*inputDevice	= nullptr;
bandHandler	the_bandHandler;
int32_t		tunedFrequency	= 220000000;	// just a setting
#ifdef	HAVE_HACKRF
int		lnaGain		= 40;
int		vgaGain		= 40;
int		ppmOffset	= 0;
bool		ampEnable	= false;
const char	*optionsString	= "D:d:M:B:C:O:R:G:g:Ap:";
#elif	HAVE_LIME
int16_t		gain		= 70;
std::string	antenna		= "Auto";
const char	*optionsString	= "D:d:M:B:C:O:R:G:X:";
#elif	HAVE_SDRPLAY	
int16_t		GRdB		= 30;
int16_t		lnaState	= 2;
bool		autoGain	= false;
int16_t		ppmOffset	= 0;
const char	*optionsString	= "D:d:M:B:C:O:R:G:L:Qp:";
#elif	HAVE_SDRPLAY_V3	
int16_t		GRdB		= 30;
int16_t		lnaState	= 2;
bool		autogain	= false;
int16_t		ppmOffset	= 0;
const char	*optionsString	= "D:d:M:B:C:O:R:G:L:Qp:";
#elif	HAVE_AIRSPY
int16_t		gain		= 20;
bool		autogain	= false;
bool		rf_bias		= false;
const char	*optionsString	= "D:d:M:B:C:O:R:G:p:B";
#elif	HAVE_RTLSDR
int16_t		gain		= 50;
bool		autogain	= false;
int16_t		ppmOffset	= 0;
const char	*optionsString	= "D:d:M:B:C:O:R:G:Qp:";
#elif	HAVE_WAVFILES
std::string	fileName;
bool		repeater	= true;
const char	*optionsString	= "D:d:M:B:O:F:r";
#elif	HAVE_RAWFILES
std::string	fileName;
bool	repeater		= true;
const char	*optionsString	= "D:d:M:B:O:F:r";
#elif
//	HAVE_RTL_TCP
int		gain		= 50;
bool		autogain	= false;
int		ppmOffset	= 0;
std::string	hostname = "127.0.0.1";		// default
int32_t		basePort = 1234;		// default
const char	*optionsString	= "D:d:M:B:C:O:R:G:Qp:H:I";
#endif
#ifdef	HAVE_DUMPING
SNDFILE		*dumpFile	= nullptr;
#endif
int	opt;
struct sigaction sigact;
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
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
#elif defined (HAVE_RTL_TCP)
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
#elif defined (HAVE_SDRPLAY) 
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
#elif defined (HAVE_HACKRF)
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
#elif defined (HAVE_AIRSPY)
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
#elif defined (HAVE_RTLSDR)
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
#elif defined (HAVE_LIME)
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
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
	         if (etiFile == nullptr) {
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
	            if (dumpFile == nullptr) {
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

	      case 'r':
	         continue_on_eof	= true;
	         fprintf (stderr, "continue_on_eof is now set\n");
	         break;
#elif defined (HAVE_RTL_TCP)
	      case 'H':
	         hostname	= std::string (optarg);
	         break;

	      case 'I':
	         basePort	= atoi (optarg);
	         break;
#else
	      case 'B':
	         theBand = std::string (optarg) == std::string ("L_BAND") ?
	                                     L_BAND : BAND_III;
	         break;

	      case 'C':
	         theChannel	= std::string (optarg);
	         break;

#if defined (HAVE_SDRPLAY)
	      case 'G':
	         GRdB	= atoi (optarg);
	         break;

	      case 'L':
	         lnaState	= atoi (optarg);
	         break;

	      case 'Q':
	         autoGain	= true;
	         break;

	      case 'p':
	         ppmOffset	= atoi (optarg);
	         break;

#elif defined (HAVE_HACKRF)
	      case 'G':
	         lnaGain	= atoi (optarg);
	         break;

	      case 'g':
	         vgaGain	= atoi (optarg);
	         break;

	      case 'p':
	         ppmOffset	= atoi (optarg);
	         break;

	      case 'A':
	         ampEnable	= true;
	         break;

#elif defined (HAVE_RTLSDR)
	      case 'G':
	         deviceGain	= atoi (optarg);
	         break;
	      case 'p':
	         ppmOffset	= atoi (optarg);
	         break;
	      case 'Q':
	         autoGain	= true;
	         break;
#elif	defined (HAVE_AIRSPY)
	      case 'G':
	         deviceGain	= atoi (optarg);
	         break;

	      case 'B':
	         rf_bias	= true;
	         break;

#elif	defined(HAVE_LIME)
              case 'G':
              case 'g':
                 gain           = atoi (optarg);
                 break;

              case 'X':
                 antenna        = std::string (optarg);
                 break;
#endif
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
	                                     ppmOffset,
	                                     deviceGain,
	                                     autoGain,
	                                     deviceIndex);
#elif	HAVE_SDRPLAY
	   inputDevice	= new sdrplayHandler (tunedFrequency,
	                                      ppmOffset,
	                                      GRdB,
	                                      lnaState,
	                                      autoGain, 0, 0);
#elif	HAVE_HACKRF
	   (void)autoGain;
	   inputDevice	= new hackrfHandler (tunedFrequency,
	                                     ppmOffset,
	                                     lnaGain,
	                                     vgaGain,
	                                     ampEnable);
#elif	HAVE_AIRSPY
	   inputDevice	= new airspyHandler (tunedFrequency,
	                                     deviceGain, autoGain, rf_bias);
#elif	HAVE_LIME
	   inputDevice	= new limeHandler   (tunedFrequency, gain, antenna);
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
	                                     ppmOffset);
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
	sigaction(SIGINT, &sigact, nullptr);
//	sigaction(SIGTERM, &sigact, nullptr);
//	sigaction(SIGQUIT, &sigact, nullptr);

//
//	do_process (channel);
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
	                    nullptr);

	inputDevice	-> restartReader (tunedFrequency);
	timesyncSet. store (false);
	theWorker. start_ofdmProcessing ();
	while (!timeSynced. load () && (--timeSyncTime >= 0)) 
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
	if (etiFile != nullptr)
	   fclose (etiFile);
#ifdef	HAVE_DUMPFILE
	if (dumpFile != nullptr)
	   sf_close (dumpFile);
#endif
	exit (1);
}

void    printOptions (void) {
        std::cerr << 
" general eti-cmdline-xxx options are\n"
"\n"
"   -D number   time (in seconds) to look for a DAB ensemble\n"
"   -M mode     Mode to be used "
"   -B Band     select DAB Band (default: BAND_III, or L_BAND)\n"
"   -C channel  DAB channel to be used (5A ... 13F resp. LA ... LP)\n"
"   -O filename write output into a file (instead of stdout)\n"
"   -R filename (if configured) dump to an *.sdr file\n"
"   -h          show options and quit\n"; 

#ifdef	HAVE_WAVFILES
	std::cerr << 
"   -F filename, -r repeat after reaching eof\n";
#elif	HAVE_RAWFILES
	std::cerr << 
"   -F filename, -r repeat after reaching eof\n";
#elif	HAVE_SDRPLAY
	std::cerr <<
"   -G number ifgain reduction (20 .. 59), \n"
"   -L number lna state selection \n"
"   -Q autogain on \n"
"   -p number ppm correction \n";
#elif	HAVE_AIRSPY
	std::cerr <<
"   -G number Gain (combined gain in the range 1 .. 21) \n"
"   -B bias on\n";
#elif	HAVE_HACKRF
	std::cerr <<
"   -G number lna gain \n"
"   -g number vga gain \n"
"   -A ampEnable on\n"
"   -p number ppm correction\n";
#elif	HAVE_RTLSDR
	std::cerr <<
"   -G number gain setting, depending on the version of the stick \n"
"   -p number ppm setting \n"
"   -Q autogain on\n";
#elif	HAVE_LIME
	std::cerr << 
"   -G number gain setting \n"
"   -X string antenna setting\n";
#elif	HAVE_RTLTCP
	std::cerr <<
"    -G number gain setting \n"
"    -Q autogain on \n"
"    -p number ppm correction \n"
"    -H string hostname \n"
"    -I number baseport\n";
#endif
}

