#
/*
 *    Copyright (C) 2013, 2014, 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  SDR-J (JSDR).
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are acknowledged.
 *
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<QSettings>
#include	<QMessageBox>
#include	<QFileDialog>
#include	<QDebug>
#include	<QDateTime>
#include	<QFile>
#include	<QStringList>
#include	<QStringListModel>
#include	"dab-constants.h"
#include	"gui.h"
#include	"eti-generator.h"
#include	"rawfiles.h"
#include	"wavfiles.h"
#ifdef	HAVE_DABSTICK
#include	"dabstick.h"
#endif
#ifdef	HAVE_SDRPLAY
#include	"sdrplay.h"
#endif
#ifdef	HAVE_UHD
#include	"uhd-input.h"
#endif
#ifdef	HAVE_EXTIO
#include	"extio-handler.h"
#endif
#ifdef	HAVE_RTL_TCP
#include	"rtl_tcp_client.h"
#endif
#ifdef	HAVE_AIRSPY
#include	"airspy-handler.h"
#endif
/**
  *	We use the creation function merely to set up the
  *	user interface and make the connections between the
  *	gui elements and the handling agents. All real action
  *	is embedded in actions, initiated by gui buttons
  */
	RadioInterface::RadioInterface (QSettings	*Si,
	                                uint8_t		dabMode,
	                                QString		dabBand,
	                                uint8_t		freqsyncMethod,
	                                FILE		*eti_file,
	                                QWidget		*parent): QMainWindow (parent) {
int16_t	latency;

// 	the setup for the generated part of the ui
	setupUi (this);
	dabSettings		= Si;
	this	-> eti_file	= eti_file;
//
//	Before printing anything, we set
	setlocale (LC_ALL, "");

	inputDevice		= new virtualInput ();
	running			= false;
	eti_running		= false;
	
/**	threshold is used in the phaseReference class 
  *	as threshold for checking the validity of the correlation result
  */
	threshold	=
	           dabSettings -> value ("threshold", 3). toInt ();


	isSynced		= UNSYNCED;
//
	this -> dabBand		= dabBand == "BAND III" ? BAND_III : L_BAND;
	setupChannels	(channelSelector, this -> dabBand);
	this	-> dabMode	= dabMode;
	setModeParameters (dabMode);
/**
  */
	this	-> freqsyncMethod	= freqsyncMethod;
	my_etiGenerator		= new etiGenerator	(this,
	                                                 &dabModeParameters);

	my_ofdmProcessor	= new ofdmProcessor   (inputDevice,
	                                               &dabModeParameters,
	                                               this,
	                                               my_etiGenerator,
	                                               threshold,
	                                               freqsyncMethod);
	init_your_gui ();		// gui specific stuff
}

	RadioInterface::~RadioInterface (void) {
	fprintf (stderr, "deleting radioInterface\n");
}
//
/**
  *	\brief At the end, we might save some GUI values
  *	The QSettings could have been the class variable as well
  *	as the parameter
  */
void	RadioInterface::dumpControlState (QSettings *s) {
	if (s == NULL)	// cannot happen
	   return;

	s	-> setValue ("channel",
	                      channelSelector -> currentText ());
	s	-> setValue ("device", deviceSelector -> currentText ());
}
//
///	the values for the different Modes:
void	RadioInterface::setModeParameters (uint8_t Mode) {
	if (Mode == 2) {
	   dabModeParameters. dabMode	= 2;
	   dabModeParameters. L		= 76;		// blocks per frame
	   dabModeParameters. K		= 384;		// carriers
	   dabModeParameters. T_null	= 664;		// null length
	   dabModeParameters. T_F	= 49152;	// samples per frame
	   dabModeParameters. T_s	= 638;		// block length
	   dabModeParameters. T_u	= 512;		// useful part
	   dabModeParameters. guardLength	= 126;
	   dabModeParameters. carrierDiff	= 4000;
	} else
	if (Mode == 4) {
	   dabModeParameters. dabMode		= 4;
	   dabModeParameters. L			= 76;
	   dabModeParameters. K			= 768;
	   dabModeParameters. T_F		= 98304;
	   dabModeParameters. T_null		= 1328;
	   dabModeParameters. T_s		= 1276;
	   dabModeParameters. T_u		= 1024;
	   dabModeParameters. guardLength	= 252;
	   dabModeParameters. carrierDiff	= 2000;
	} else 
	if (Mode == 3) {
	   dabModeParameters. dabMode		= 3;
	   dabModeParameters. L			= 153;
	   dabModeParameters. K			= 192;
	   dabModeParameters. T_F		= 49152;
	   dabModeParameters. T_null		= 345;
	   dabModeParameters. T_s		= 319;
	   dabModeParameters. T_u		= 256;
	   dabModeParameters. guardLength	= 63;
	   dabModeParameters. carrierDiff	= 2000;
	} else {	// default = Mode I
	   dabModeParameters. dabMode		= 1;
	   dabModeParameters. L			= 76;
	   dabModeParameters. K			= 1536;
	   dabModeParameters. T_F		= 196608;
	   dabModeParameters. T_null		= 2656;
	   dabModeParameters. T_s		= 2552;
	   dabModeParameters. T_u		= 2048;
	   dabModeParameters. guardLength	= 504;
	   dabModeParameters. carrierDiff	= 1000;
	}
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
  *	\brief setupChannels
  *	sets the entries in the GUI
  */
//
//	Note that the ComboBox is GUI specific, but we assume
//	a comboBox is available to act later on as selector
//	for the channels
//
void	RadioInterface::setupChannels (QComboBox *s, uint8_t band) {
struct dabFrequencies *t;
int16_t	i;
int16_t	c	= s -> count ();

//	clear the fields in the conboBox
	for (i = 0; i < c; i ++) 
	   s	-> removeItem (c - (i + 1));

	if (band == BAND_III)
	   t = bandIII_frequencies;
	else
	   t = Lband_frequencies;

	for (i = 0; t [i]. key != NULL; i ++) 
	   s -> insertItem (i, t [i]. key, QVariant (i));
}

void	RadioInterface::init_your_gui (void) {
	ficBlocks		= 0;
	ficSuccess		= 0;
	syncedLabel		->
	               setStyleSheet ("QLabel {background-color : red}");
/**
  *	Devices can be included or excluded, setting is in the configuration
  *	files. Inclusion is reflected in the selector on the GUI.
  *	Note that HAVE_EXTIO is only useful for Windows
  */
#ifdef	HAVE_SDRPLAY
	deviceSelector	-> addItem ("sdrplay");
#endif
#ifdef	HAVE_DABSTICK
	deviceSelector	-> addItem ("dabstick");
#endif
#ifdef	HAVE_AIRSPY
	deviceSelector	-> addItem ("airspy");
#endif
#ifdef HAVE_UHD
	deviceSelector	-> addItem("UHD");
#endif
#ifdef HAVE_EXTIO
	deviceSelector	-> addItem("extio");
#endif
#ifdef	HAVE_RTL_TCP
	deviceSelector	-> addItem ("rtl_tcp");
#endif
	
	connect (startButton, SIGNAL (clicked (void)),
	              this, SLOT (setStart (void)));
	connect (quitButton, SIGNAL (clicked ()),
	              this, SLOT (TerminateProcess (void)));
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (setDevice (const QString &)));
	connect (channelSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (set_channelSelect (const QString &)));
	connect (dumpButton, SIGNAL (clicked (void)),
	              this, SLOT (set_dumping (void)));
	connect (resetButton, SIGNAL (clicked (void)),
	              this, SLOT (autoCorrector_on (void)));
	connect (eti_control, SIGNAL (clicked (void)),
	              this, SLOT (set_etiControl (void)));
/**	
  *	Happily, Qt is very capable of handling the representation
  *	of the ensemble and selecting an item
  */
	ensemble.setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);
	Services << " ";
	ensemble. setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);

/**
  *	The only timer we use is for displaying the running time.
  *	The number of seconds passed is kept in numberofSeconds
  */	
	numberofSeconds		= 0;
	displayTimer		= new QTimer ();
	displayTimer		-> setInterval (1000);
	connect (displayTimer,
	         SIGNAL (timeout (void)),
	         this,
	         SLOT (updateTimeDisplay (void)));
//
	sourceDumping		= false;
	dumpfilePointer		= NULL;
//
/**
  *	we now handle the settings as saved by previous incarnations.
  */
	setDevice 		(deviceSelector 	-> currentText ());
	QString h		=
	           dabSettings -> value ("device", "no device"). toString ();
	if (h == "no device")	// no autostart here
	   autoStart = false;
	int k		= deviceSelector -> findText (h);
	if (k != -1) {
	   deviceSelector	-> setCurrentIndex (k);
	   setDevice 		(deviceSelector 	-> currentText ());
	}

	h		= dabSettings -> value ("channel", "12C"). toString ();
	k		= channelSelector -> findText (h);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	   set_channelSelect (h);
	}
	else
	   autoStart	= false;
	
//	display the version
	QString v = "dab-eti (experimental)  " ;
	versionName	-> setText (v);
//	and start the timer
	displayTimer		-> start (1000);
}


///////////////////////////////////////////////////////////////////////////////
//	
//	The public slots are called from other places within the dab software
//	so please provide some implementation, perhaps an empty one
//
//	a slot called by the ofdmprocessor
void	RadioInterface::set_fineCorrectorDisplay (int v) {
	finecorrectorDisplay	-> display (v);
}

//	a slot called by the ofdmprocessor
void	RadioInterface::set_coarseCorrectorDisplay (int v) {
	coarsecorrectorDisplay	-> display (v);
}
/**
  *	clearEnsemble
  *	on changing settings, we clear all things in the gui
  *	related to the ensemble.
  *	The function is called from "deep" within the handling code
  *	Potentially a dangerous approach, since the fic handler
  *	might run in a separate thread and generate data to be displayed
  */
void	RadioInterface::clearEnsemble	(void) {
//
//	it obviously means: stop processing
	my_etiGenerator		-> stopProcessing ();
	eti_control		-> setText ("eti waiting");
	my_ofdmProcessor	-> coarseCorrectorOn ();
	my_ofdmProcessor	-> reset ();

	clear_showElements	();
}

//
//	a slot, called by the fic/fib handlers
void	RadioInterface::addtoEnsemble (const QString &s) {
	Services << s;
	Services. removeDuplicates ();
	ensemble. setStringList (Services);
	ensembleDisplay	-> setModel (&ensemble);
}

//
///	a slot, called by the fib processor
void	RadioInterface::nameofEnsemble (int id, const QString &v) {
QString s;
	(void)v;
	ensembleId		-> display (id);
	ensembleLabel		= v;
	ensembleName		-> setText (v);
	my_ofdmProcessor	-> coarseCorrectorOff ();
}

///	... and the same for the FIC blocks
void	RadioInterface::show_ficCRC (bool b) {
	if (b)
	   ficSuccess ++;
	if (++ficBlocks >= 100) {
	   ficRatioDisplay	-> display (ficSuccess);
	   ficSuccess	= 0;
	   ficBlocks	= 0;
	}
}

///	called from the ofdmDecoder, which computed this for each frame
void	RadioInterface::show_snr (int s) {
	snrDisplay	-> display (s);
}

///	just switch a color, obviously GUI dependent, but called
//	from the ofdmprocessor
void	RadioInterface::setSynced	(char b) {
	if (isSynced == b)
	   return;

	isSynced = b;
	switch (isSynced) {
	   case SYNCED:
	      syncedLabel -> 
	               setStyleSheet ("QLabel {background-color : green}");
	      break;

	   default:
	      syncedLabel ->
	               setStyleSheet ("QLabel {background-color : red}");
	      break;
	}
}

/**
  *	\brief changeinConfiguration
  *	No idea yet what to do, so just give up
  *	with what we were doing. The user will -eventually -
  *	see the new configuration from which he can select
  */
void	RadioInterface::changeinConfiguration	(void) {
	if (running) {
	   inputDevice		-> stopReader ();
	   inputDevice		-> resetBuffer ();
	   running		= false;
	}
	clear_showElements	();
}

void	RadioInterface::clear_showElements (void) {
	Services = QStringList ();
	ensemble. setStringList (Services);
	ensembleDisplay		-> setModel (&ensemble);

	ensembleLabel		= QString ();
	ensembleName		-> setText (ensembleLabel);
	
//	Then the various displayed items
	ensembleName		-> setText ("   ");
	ficRatioDisplay		-> display (0);
	snrDisplay		-> display (0);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//	
//	Private slots relate to the modeling of the GUI
//
//
void	RadioInterface::setStart	(void) {
bool	r = 0;
	if (running)		// only listen when not running yet
	   return;
//
	r = inputDevice		-> restartReader ();
	qDebug ("Starting %d\n", r);
	if (!r) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Opening  input stream failed\n"));
	   return;
	}
//
//	Of course, starting the machine will generate a new instance
//	of the ensemble, so the listing - if any - should be cleared
	clearEnsemble ();		// the display
	running = true;
}

/**
  *	\brief TerminateProcess
  *	Pretty critical, since there are many threads involved
  *	A clean termination is what is needed, regardless of the GUI
  */
void	RadioInterface::TerminateProcess (void) {
	running		= false;
	displayTimer	-> stop ();
	if (sourceDumping) {
	   my_ofdmProcessor	-> stopDumping ();
	   sf_close (dumpfilePointer);
	}

	inputDevice		-> stopReader ();	// might be concurrent
	my_etiGenerator		-> stopProcessing ();	// might be concurrent
	my_ofdmProcessor	-> stop ();		// definitely concurrent
//
//	everything should be halted by now
	dumpControlState (dabSettings);
//	delete		my_ofdmProcessor;
	delete		my_etiGenerator;
	delete		displayTimer;
	delete		inputDevice;
	close ();
	fprintf (stderr, "closed\n");
}

//
/**
  *	\brief set_channelSelect
  *	Depending on the GUI the user might select a channel
  *	or some magic will cause a channel to be selected
  */
void	RadioInterface::set_channelSelect (QString s) {
int16_t	i;
struct dabFrequencies *finger;
bool	localRunning	= running;
int32_t	tunedFrequency;

	if (localRunning) {
	   inputDevice		-> stopReader ();
	   inputDevice		-> resetBuffer ();
	   if (eti_running) {
	      my_etiGenerator	-> stopProcessing ();
	      eti_control	-> setText ("eti waits");
	      eti_running	= false;
	   }
	}

	clear_showElements ();

	tunedFrequency		= 0;
	if (dabBand == BAND_III)
	   finger = bandIII_frequencies;
	else
	   finger = Lband_frequencies;

	for (i = 0; finger [i]. key != NULL; i ++) {
	   if (finger [i]. key == s) {
	      tunedFrequency	= KHz (finger [i]. fKHz);
	      break;
	   }
	}

	if (tunedFrequency == 0)
	   return;

	inputDevice		-> setVFOFrequency (tunedFrequency);

	if (localRunning) {
	   inputDevice	 -> restartReader ();
	   my_ofdmProcessor	-> reset ();
	   my_etiGenerator	-> stopProcessing ();
	   running	 = true;
	}
}

void	RadioInterface::updateTimeDisplay (void) {
	numberofSeconds ++;
	int16_t	numberHours	= numberofSeconds / 3600;
	int16_t	numberMinutes	= (numberofSeconds / 60) % 60;
	QString text = QString ("runtime ");
	text. append (QString::number (numberHours));
	text. append (" hr, ");
	text. append (QString::number (numberMinutes));
	text. append (" min");
	timeDisplay	-> setText (text);
}

void	RadioInterface::autoCorrector_on (void) {
//	first the real stuff
	clear_showElements	();
	my_ofdmProcessor	-> coarseCorrectorOn ();
	my_ofdmProcessor	-> reset ();
	my_etiGenerator		-> stopProcessing ();
}
//
/**
  *	\brief setDevice
  *	setDevice is called trough a signal from the gui
  *	Operation is in three steps: 
  *	   first dumping of any kind is stopped
  *	   second the previously loaded device is stopped
  *	   third, the new device is initiated, but not started
  */
//
//	setDevice is called from the GUI. Other GUI's might have a preselected
//	single device to go with, then if suffices to extract some
//	code specific to that device
void	RadioInterface::setDevice (QString s) {
bool	success;
QString	file;
//
///	first stop dumping

	if (sourceDumping) {
	   my_ofdmProcessor -> stopDumping ();
	   sf_close (dumpfilePointer);
	   sourceDumping = false;
	   dumpButton	-> setText ("dump");
	}

//	indicate that we are not running anymore
	running	= false;
	inputDevice	-> stopReader ();
	delete	my_ofdmProcessor;
	delete	inputDevice;
///	OK, everything quiet, now looking what to do
#ifdef	HAVE_AIRSPY
	if (s == "airspy") {
	   inputDevice	= new airspyHandler (dabSettings, &success);
	   if (!success) {
	      delete inputDevice;
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("airspy: no luck\n"));
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect	(channelSelector -> currentText ());
	}
	else
#endif
#ifdef HAVE_UHD
//	UHD is - at least in its current setting - for Linux
//	and not tested by me
	if (s == "UHD") {
	   inputDevice = new uhdInput (dabSettings, &success );
	   if (!success) {
	      delete inputDevice;
	      QMessageBox::warning( this, tr ("sdr"), tr ("UHD: no luck\n") );
	      inputDevice = new virtualInput();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect (channelSelector->currentText() );
	}
	else
#endif
#ifdef HAVE_EXTIO
//	extio is - in its current settings - for Windows, it is a
//	wrap around the dll
	if (s == "extio") {
	   inputDevice = new extioHandler (dabSettings, &success);
	   if (!success) {
	      delete inputDevice;
	      QMessageBox::warning( this, tr ("sdr"), tr ("extio: no luck\n") );
	      inputDevice = new virtualInput();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect (channelSelector -> currentText() );
	}
	else
#endif
#ifdef HAVE_RTL_TCP
//	RTL_TCP might be working. 
	if (s == "rtl_tcp") {
	   inputDevice = new rtl_tcp_client (dabSettings, &success);
	   if (!success) {
	      delete inputDevice;
	      QMessageBox::warning( this, tr ("sdr"), tr ("UHD: no luck\n") );
	      inputDevice = new virtualInput();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect (channelSelector->currentText() );
	}
	else
#endif
#ifdef	HAVE_SDRPLAY
	if (s == "sdrplay") {
	   inputDevice	= new sdrplay (dabSettings, &success);
	   if (!success) {
	      delete inputDevice;
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("SDRplay: no library\n"));
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect	(channelSelector -> currentText ());
	}
	else
#endif
#ifdef	HAVE_DABSTICK
	if (s == "dabstick") {
	   inputDevice	= new dabStick (dabSettings, &success);
	   if (!success) {
	      delete inputDevice;
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Dabstick: no luck\n"));
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	   else 
	      set_channelSelect	(channelSelector -> currentText ());
	}
	else
#endif
//
//	We always have fileinput!!
	if (s == "file input (.raw)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("open file ..."),
	                                                QDir::homePath (),
	                                                tr ("raw data (*.raw)"));
	   file		= QDir::toNativeSeparators (file);
	   inputDevice	= new rawFiles (file, &success);
	   if (!success) {
	      delete inputDevice;
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	}
	else
	if (s == "file input (.sdr)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("open file ..."),
	                                                QDir::homePath (),
	                                                tr ("raw data (*.sdr)"));
	   file		= QDir::toNativeSeparators (file);
	   inputDevice	= new wavFiles (file, &success);
	   if (!success) {
	      delete inputDevice;
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	}
	else {	// s == "no device" 
//	and as default option, we have a "no device"
	   inputDevice	= new virtualInput ();
	}
///	we have a new device, so we can re-create the ofdmProcessor
///	Note: the fichandler and etiGenerator remain unchanged
	my_ofdmProcessor	= new ofdmProcessor   (inputDevice,
	                                               &dabModeParameters,
	                                               this,
	                                               my_etiGenerator,
	                                               threshold,
	                                               freqsyncMethod);
}

void	RadioInterface::resetSelector (void) {
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            this, SLOT (setDevice (const QString &)));
int	k	= deviceSelector -> findText (QString ("no device"));
	if (k != -1) { 		// should not happen
	   deviceSelector -> setCurrentIndex (k);
	}
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (setDevice (const QString &)));
}

//	Dumping is GUI dependent and may be ignored
///	switch for dumping on/off
void	RadioInterface::set_dumping (void) {
SF_INFO *sf_info	= (SF_INFO *)alloca (sizeof (SF_INFO));

	if (!someStick (inputDevice -> myIdentity ()))
	   return;

	if (sourceDumping) {
	   my_ofdmProcessor	-> stopDumping ();
	   sf_close (dumpfilePointer);
	   sourceDumping = false;
	   dumpButton	-> setText ("dump");
	   return;
	}

	QString file = QFileDialog::getSaveFileName (this,
	                                     tr ("open file ..."),
	                                     QDir::homePath (),
	                                     tr ("raw data (*.sdr)"));
	file	= QDir::toNativeSeparators (file);
	if (!file. endsWith (".sdr", Qt::CaseInsensitive))
	   file. append (".sdr");
	sf_info	-> samplerate	= INPUT_RATE;
	sf_info	-> channels	= 2;
	sf_info	-> format	= SF_FORMAT_WAV | SF_FORMAT_PCM_16;

	dumpfilePointer	= sf_open (file. toLatin1 (). data (),
	                                   SFM_WRITE, sf_info);
	if (dumpfilePointer == NULL) {
	   qDebug () << "cannot open " << file. toLatin1 (). data ();
	   return;
	}
	dumpButton	-> setText ("writing");
	sourceDumping		= true;
	my_ofdmProcessor -> startDumping (dumpfilePointer);
}

void	RadioInterface::set_etiControl	(void) {
QString	filename;
	if (!eti_running) {
	   deviceSelector	-> hide ();
	   channelSelector	-> hide ();
	   if (eti_file == NULL) {
	      filename	= QFileDialog::getSaveFileName (this,
                                                tr ("open file .."),
                                                QDir::homePath (),
                                                tr ("Eti (*.eti)"));
              filename	= QDir::toNativeSeparators (filename);
              if (!filename. endsWith (".wav", Qt::CaseInsensitive))
                 filename. append (".eti");
	      eti_file	= fopen (filename. toLatin1 (). data (), "w");
	   }
	   my_etiGenerator	-> startProcessing (eti_file);
	   eti_control		-> setText ("eti is working");
	   eti_running		= true;
	}
	else {
	   if (eti_running) {
	      my_etiGenerator	-> stopProcessing ();
	   }
	   eti_control		-> setText ("eti waits");
	   eti_running		= false;
	   deviceSelector	-> show ();
	   channelSelector	-> show ();
	}
}


