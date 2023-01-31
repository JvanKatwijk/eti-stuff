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
#if !(defined(__MINGW32__) || defined(_WIN32))
#include        <unistd.h>
#include	<getopt.h>
#endif

#include	<signal.h>
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


#if (defined(__MINGW32__) || defined(_WIN32))
#include <string.h>
#include <stdio.h>

int     opterr = 1,             /* if error message should be printed */
        optind = 1,             /* index into parent argv vector */
        optopt,                 /* character checked for validity */
        optreset;               /* reset getopt */
char    *optarg;                /* argument associated with option */

#define BADCH   (int)'?'
#define BADARG  (int)':'
#define EMSG    ""

/*
 * getopt --
 *      Parse argc/argv argument vector.
 */
int getopt (int nargc, char * const nargv[], const char *ostr) {
static char *place = EMSG;	/* option letter processing */
const char *oli; 		/* option letter list index */

	if (optreset || !*place) {              /* update scanning pointer */
	   optreset = 0;
	   if (optind >= nargc || *(place = nargv[optind]) != '-') {
	      place = EMSG;
	      return (-1);
	   }

	   if (place[1] && *++place == '-') {      /* found "--" */
	      ++optind;
	      place = EMSG;
	      return (-1);
	   }
	}                                       /* option letter okay? */

	if ((optopt = (int)*place++) == (int)':' ||
	                   !(oli = strchr(ostr, optopt))) {
/*
 * if the user didn't specify '-' as an option,
 * assume it means -1.
 */
	   if (optopt == (int)'-')
	      return (-1);
	   if (!*place)
	      ++optind;
	   if (opterr && *ostr != ':')
	      (void)printf ("illegal option -- %c\n", optopt);
	   return (BADCH);
	}

	if (*++oli != ':') {	// single letter without additionals
	   optarg = NULL;
	   if (!*place)
	      ++optind;
	}
	else { 			// optarg is following
	   if (*place)           /* no white space */
	      optarg = place;
	   else
	   if (nargc <= ++optind) {   /* no arg */
	      place = EMSG;
	      if (*ostr == ':')
	         return (BADARG);
	      if (opterr)
	         (void)printf ("option requires an argument -- %c\n", optopt);
	      return (BADCH);
	   }
	   else                      /* white space */
	      optarg = nargv[optind];
	   place = EMSG;
	   ++optind;
	}
	return (optopt);                        /* dump back option letter */
}

#endif
using std::cerr;
using std::endl;
//
#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler/rtlsdr-handler.h"
#elif	HAVE_SDRPLAY
#include	"sdrplay-handler.h"
#elif	HAVE_SDRPLAY_V3
#include	"sdrplay-handler-v3.h"
#elif	HAVE_AIRSPY
#include	"airspy-handler.h"
#elif	HAVE_HACKRF
#include	"hackrf-handler.h"
#elif	HAVE_PLUTO
#include	"pluto-handler.h"
#elif	HAVE_LIME
#include	"lime-handler.h"
#elif	HAVE_RAWFILES
#include	"rawfile-handler.h"
#elif	HAVE_WAVFILES
#include	"wavfile-handler.h"
#elif	HAVE_RTL_TCP
#include	"rtl_tcp-client.h"
#elif	HAVE_XMLFILES
#include	"xml-filereader.h"	// does not work yet
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

#if defined(__MINGW32) || defined( _WIN32)
BOOL WINAPI
sighandler (int signum) {
	if (CTRL_C_EVENT == signum) {
	   fprintf(stderr, "Signal caught, exiting!\n");
	   run. store (false);
	   return TRUE;
	}
	return FALSE;
}
#else
static void sighandler (int signum) {
        fprintf (stderr, "\n\nSignal %d caught, terminating!\n", signum);
	run. store (false);
}
#endif
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
static FILE	*etiFile;
static int	cnt	= 0;
void	etiwriterHandler (uint8_t *buffer, int32_t amount, void *ctx) {
	(void)ctx;
	fwrite (buffer, 1, amount, etiFile);
	if (!isSilent)
	   fprintf (stderr, "%d\r", ++cnt);
}

void	inputStoppedHandler	(void) {
	run. store (false);
}

void    printOptions (void);

callbacks	the_callBacks;

int	main (int argc, char **argv) {
// Default values
int16_t		timeSyncTime	= 5;
int16_t		freqSyncTime	= 10;
uint8_t		theMode		= 1;
int		nrProcessors	= 4;
std::string	theChannel	= "11C";
uint8_t		theBand		= BAND_III;
deviceHandler	*inputDevice	= nullptr;
bandHandler	the_bandHandler;
int32_t		tunedFrequency	= 220000000;	// just a setting
int		recordTime	= -1;
#ifdef	HAVE_HACKRF
int		lnaGain		= 40;
int		vgaGain		= 40;
int		ppmOffset	= 0;
bool		ampEnable	= false;
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:g:Ap:";
#elif	HAVE_PLUTO
int		plutoGain	= 50;
bool		pluto_agc	= false;
bool		filter_on	= true;
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:QF";
#elif	HAVE_LIME
int16_t		gain		= 70;
std::string	antenna		= "Auto";
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:X:";
#elif	HAVE_SDRPLAY	
int16_t		GRdB		= 30;
int16_t		lnaState	= 2;
bool		autoGain	= false;
int16_t		ppmOffset	= 0;
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:L:Qp:";
#elif	HAVE_SDRPLAY_V3	
int16_t		GRdB		= 30;
int16_t		lnaState	= 2;
bool		autoGain	= false;
int16_t		ppmOffset	= 0;
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:L:Qp:";
#elif	HAVE_AIRSPY
int16_t		deviceGain	= 20;
bool		autoGain	= false;
bool		rf_bias		= false;
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:p:b";
#elif	HAVE_RTLSDR
int16_t		deviceGain	= 50;
bool		autoGain	= false;
int16_t		ppmOffset	= 0;
int		deviceIndex	= 0;
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:Qp:";
#elif	HAVE_WAVFILES
std::string	fileName;
bool		repeater	= true;
bool		continue_on_eof	= false;
const char	*optionsString	= "ShP:D:d:M:B:O:F:rt:";
#elif	HAVE_RAWFILES
std::string	fileName;
bool		repeater	= true;
bool		continue_on_eof	= false;
const char	*optionsString	= "ShP:D:d:M:B:O:F:rt:";
#elif	HAVE_XMLFILES
std::string	fileName;
bool		repeater	= true;
bool		continue_on_eof	= false;
const char	*optionsString	= "ShP:D:d:M:B:O:F:rt:";
#elif	HAVE_RTL_TCP
int		deviceGain	= 50;
bool		autoGain	= false;
int		ppmOffset	= 0;
std::string	hostname = "127.0.0.1";		// default
int32_t		basePort = 1234;		// default
const char	*optionsString	= "ShP:D:d:M:B:C:O:R:t:G:Qp:H:I:";
#endif
#ifdef	HAVE_DUMPING
SNDFILE		*dumpFile	= nullptr;
#endif
int	opt;
#if !(defined(__MINGW32) || defined(_WIN32))
struct sigaction sigact;
#endif
//
//	default
	etiFile		= stdout;
	std::setlocale (LC_ALL, "");
	timeSynced.		store (false);
	timesyncSet.		store (false);
	ensembleRecognized.	store (false);
	run.			store (false);

//	for file input some command line parameters are meeaningless
	while ((opt = getopt (argc, argv, optionsString)) != -1) {
	   switch (opt) {
	      case 'P':
	         nrProcessors	= atoi (optarg);
	         if (nrProcessors <= 0)
	            nrProcessors = 1;
	         break;

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

		  case 't':
	         recordTime	= atoi (optarg);
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

#if	defined (HAVE_RAWFILES) || defined (HAVE_WAVFILES) || defined (HAVE_XMLFILES)
	      case 'F':
	         fileName	= std::string (optarg);
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
#endif
	      case 'B':
	         theBand = std::string (optarg) == std::string ("L_BAND") ?
	                                     L_BAND : BAND_III;
	         break;

	      case 'C':
	         theChannel	= std::string (optarg);
			   for (auto & c: theChannel) c = toupper(c);
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

#elif defined (HAVE_SDRPLAY_V3)
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

#elif defined (HAVE_PLUTO)
	      case 'G':	
	         plutoGain	= atoi (optarg);
	         break;

	      case 'Q':
	         pluto_agc	= true;
	         break;

	      case 'F':
	         filter_on	= false;
	         break;
#elif defined (HAVE_RTLSDR) || defined (HAVE_RTL_TCP)
	      case 'G':
	      {
	         int deviceGainArg	= atoi (optarg);
	         if ((deviceGainArg >= 0) && (deviceGainArg <= 100)) {
	            deviceGain = deviceGainArg;
	         }
	         else {
	           fprintf (stderr, "Invalid gain value, using default %d\n",
	                                                     deviceGainArg);
	         }
	         break;
	      }
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

	      case 'b':
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

	the_callBacks. theWriter	= etiwriterHandler;
	the_callBacks. theEnsemble	= ensemblenameHandler;
	the_callBacks. theProgram	= programnameHandler;
	the_callBacks. theSyncSignal	= syncsignalHandler;
	the_callBacks. theSnrSignal	= snrsignalHandler;
	the_callBacks. theFibQuality	= fibqualityHandler;
	the_callBacks. theInputStopped	= inputStoppedHandler;

#if defined(HAVE_RAWFILES) || defined(HAVE_WAVFILES) || defined (HAVE_XMLFILES)
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
#elif	HAVE_SDRPLAY_V3
	   inputDevice	= new sdrplayHandler_v3 (tunedFrequency,
	                                         ppmOffset,
	                                         GRdB,
	                                         lnaState,
	                                         autoGain, 0, 0);
#elif	HAVE_SDRPLAY
	   inputDevice	= new sdrplayHandler (tunedFrequency,
	                                      ppmOffset,
	                                      GRdB,
	                                      lnaState,
	                                      autoGain, 0, 0);
#elif	HAVE_HACKRF
	   inputDevice	= new hackrfHandler (tunedFrequency,
	                                     ppmOffset,
	                                     lnaGain,
	                                     vgaGain,
	                                     ampEnable);
#elif	HAVE_PLUTO
	   inputDevice	= new plutoHandler (tunedFrequency,
	                                    plutoGain, pluto_agc);
#elif	HAVE_AIRSPY
	   inputDevice	= new airspyHandler (tunedFrequency,
	                                     deviceGain, autoGain, rf_bias);
#elif	HAVE_LIME
	   inputDevice	= new limeHandler   (tunedFrequency, gain, antenna);
#elif	HAVE_RAWFILES
	   inputDevice	= new rawfileHandler (fileName,
	                                      continue_on_eof, 
	                                      inputStoppedHandler);
#elif	HAVE_WAVFILES
	   inputDevice	= new wavfileHandler (fileName,
	                                      continue_on_eof,
	                                      inputStoppedHandler);
#elif	HAVE_XMLFILES
	   inputDevice	= new xml_fileReader (fileName,
	                                      continue_on_eof,
	                                      inputStoppedHandler);
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
#if defined( __MINGW32) || defined(_WIN32)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)sighandler, TRUE);
#else
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, nullptr);
//	sigaction(SIGTERM, &sigact, nullptr);
//	sigaction(SIGQUIT, &sigact, nullptr);
#endif
//
//	do_process (channel);
	etiClass theWorker (theMode,
	                    inputDevice,
	                    nrProcessors,
#ifdef	HAVE_DUMPING
	                    dumpFile,
#endif
	                    &the_callBacks,
	                    nullptr);

	inputDevice	-> restartReader (tunedFrequency);
	timesyncSet. store (false);
	theWorker. start_ofdmProcessing ();
	while (!timeSynced. load () && (--timeSyncTime >= 0)) 
	   special_sleep (1);

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
	   special_sleep (1);
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
	   while (run. load () && ((recordTime == -1) || recordTime > 0)) {
	      if (!isSilent)
	         fprintf (stderr, "\t\testimated snr: %2d, fibquality %3d\r",
	                            signalnoise. load (), ficSuccess. load ());
	      special_sleep (1);
	      if (recordTime != -1)
	         recordTime -= 1;
	   }
	}

//	we started the "worker", so we also stop it here
	theWorker. stop ();
	fprintf (stderr, "\n\nterminating\n");
	special_usleep (1000);

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
"\n eti-cmdline-";
#ifdef	HAVE_WAVFILES
	std::cerr << "wavfiles";
#elif	HAVE_XMLFILES
	std::cerr << "xmlfiles";
#elif	HAVE_RAWFILES
	std::cerr << "rawfiles";
#elif	HAVE_SDRPLAY
	std::cerr << "sdrplay";
#elif	HAVE_AIRSPY
	std::cerr << "airspy";
#elif	HAVE_HACKRF
	std::cerr << "hackrf";
#elif	HAVE_RTLSDR
	std::cerr << "rtlsdr";
#elif	HAVE_LIME
	std::cerr << "limesdr";
#elif	HAVE_PLUTO
	std::cerr << "pluto";
#elif	HAVE_RTL_TCP
	std::cerr << "rtl_tcp";
#endif
	std::cerr <<
" was compiled from \n"
" https://github.com/JvanKatwijk/eti-stuff/\n\n"
" It is an experimental program for creating a\n"
" stream of ETI frames from a selected DAB input channel.\n\n"
" The options are\n\n"
"   -P number   number of parallel threads for handling subchannels\n"
"   -D number   time (in seconds) to look for a DAB ensemble\n"
"   -M mode     mode to be used \n"
"   -O filename write output into a file (instead of stdout)\n";

#if !(defined(HAVE_XMLFILES) || defined(HAVE_WAVFILES) || defined(HAVE_RAWFILES))
	std::cerr <<
"   -B Band     select DAB Band (default: BAND_III, or L_BAND)\n"
"   -C channel  DAB channel to be used (5A ... 13F resp. LA ... LP)\n";
#endif

#ifdef	HAVE_DUMPFILE
	std::cerr <<
"   -R filename dump to an *.sdr file\n";
#endif
	std::cerr <<
"   -S          be silent during processing\n"
"   -t          set record time\n";

#if defined(HAVE_RAWFILES) || defined(HAVE_WAVFILES) || defined (HAVE_XMLFILES)
	std::cerr <<
"   -F filename\n"
"   -r          repeat after reaching eof\n";

#elif	HAVE_SDRPLAY
	std::cerr <<
"   -G number   ifgain reduction (20 .. 59), \n"
"   -L number   lna state selection \n"
"   -Q          autogain on \n"
"   -p number   ppm correction \n";
#elif	HAVE_AIRSPY
	std::cerr <<
"   -G number   gain (combined gain in the range 1 .. 21) \n"
"   -b          bias on\n";
#elif	HAVE_HACKRF
	std::cerr <<
"   -G number   lna gain \n"
"   -g number   vga gain \n"
"   -A          ampEnable on\n"
"   -p number   ppm correction\n";
#elif	HAVE_RTLSDR
	std::cerr <<
"   -G number   gain setting, depending on the version of the stick \n"
"   -p number   ppm setting \n"
"   -Q          autogain on\n";
#elif	HAVE_LIME
	std::cerr << 
"   -G number   gain setting \n"
"   -X string   antenna setting\n";
#elif	HAVE_PLUTO
	std::cerr <<
"   -G number   gain setting\n"
"   -Q          autogain on (default off)\n"
"   -F          filter off (default on)\n"
#elif	HAVE_RTL_TCP
	std::cerr <<
"   -G number   gain setting \n"
"   -Q          autogain on \n"
"   -p number   ppm correction \n"
"   -H string   hostname \n"
"   -I number   baseport\n";
#endif
	std::cerr <<
"\n   -h          show options and quit\n\n";
}

