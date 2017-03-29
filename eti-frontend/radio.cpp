#
/*
 *    Copyright (C) 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  eti-frontend of the SDR-J (JSDR) collection.
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
#include	"radio.h"
#include	"eti-generator.h"
#include	"rawfiles.h"
#include	"wavfiles.h"
#ifdef	HAVE_DABSTICK
#include	"dabstick.h"
#endif
#ifdef	HAVE_SDRPLAY
#include	"sdrplay.h"
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
	theBand. setupChannels	(channelSelector, this -> dabBand);
	this	-> dabMode	= dabMode;
/**
  */
	this	-> freqsyncMethod	= freqsyncMethod;
	my_etiGenerator		= new etiGenerator	(this,
	                                                 dabMode);

	my_ofdmProcessor	= new ofdmProcessor   (inputDevice,
	                                               dabMode,
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
#ifdef HAVE_EXTIO
	deviceSelector	-> addItem ("extio");
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
	QString h		=
	           dabSettings -> value ("device", "no device"). toString ();
	if (h == "no device")	// no autostart here
	   autoStart = false;
	int k		= deviceSelector -> findText (h);
	if (k != -1) {
	   deviceSelector	-> setCurrentIndex (k);
	}
	setDevice 		(deviceSelector 	-> currentText ());

	h		= dabSettings -> value ("channel", "12C"). toString ();
	k		= channelSelector -> findText (h);
	if (k != -1) {
	   channelSelector -> setCurrentIndex (k);
	   set_channelSelect (h);
	}
	else
	   autoStart	= false;
	
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

	tunedFrequency		= theBand. Frequency (dabBand, s);
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
	   try {
	      inputDevice	= new airspyHandler (dabSettings);
	      set_channelSelect	(channelSelector -> currentText ());
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("airspy: no luck\n"));
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	}
	else
#endif
#ifdef HAVE_EXTIO
//	extio is - in its current settings - for Windows, it is a
//	wrap around the dll
	if (s == "extio") {
	   try {
	      inputDevice = new extioHandler (dabSettings);
	      set_channelSelect (channelSelector -> currentText() );
	   }
	   catch (int e) {
	      QMessageBox::warning( this, tr ("sdr"), tr ("extio: no luck\n") );
	      inputDevice = new virtualInput();
	      resetSelector ();
	   }
	}
	else
#endif
#ifdef HAVE_RTL_TCP
//	RTL_TCP might be working. 
	if (s == "rtl_tcp") {
	   try {
	      inputDevice = new rtl_tcp_client (dabSettings);
	      set_channelSelect (channelSelector->currentText() );
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("sdr"), tr ("RTL_TCP: no luck\n") );
	      inputDevice = new virtualInput();
	      resetSelector ();
	   }
	}
	else
#endif
#ifdef	HAVE_SDRPLAY
	if (s == "sdrplay") {
	   try {
	      inputDevice	= new sdrplay (dabSettings);
	      set_channelSelect	(channelSelector -> currentText ());
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("SDRplay: no library or device\n"));
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	}
	else
#endif
#ifdef	HAVE_DABSTICK
	if (s == "dabstick") {
	   try {
	      inputDevice	= new dabStick (dabSettings);
	      set_channelSelect	(channelSelector -> currentText ());
	   }
	   catch (int e) {
	      QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Dabstick: no luck\n"));
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	}
	else
#endif
//	We always have fileinput!!
	if (s == "file input (.raw)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("open file ..."),
	                                                QDir::homePath (),
	                                                tr ("raw data (*.raw)"));
	   file		= QDir::toNativeSeparators (file);
	   try {
	      inputDevice	= new rawFiles (file);
	   }
	   catch (int e) {
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	}
	else
	if (s == "file input (.iq)") {
	   file		= QFileDialog::getOpenFileName (this,
	                                                tr ("open file ..."),
	                                                QDir::homePath (),
	                                                tr ("raw data (*.iq)"));
	   file		= QDir::toNativeSeparators (file);
	   try {
	      inputDevice	= new rawFiles (file);
	   }
	   catch (int e) {
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
	   try {
	      inputDevice	= new wavFiles (file);
	   }
	   catch (int e) {
	      inputDevice = new virtualInput ();
	      resetSelector ();
	   }
	}
	else {	// s == "no device" 
//	and as default option, we have an always existing "no device"
	   inputDevice	= new virtualInput ();
	}
///	we have a new device, so we can re-create the ofdmProcessor
///	Note: the fichandler and etiGenerator remain unchanged
	my_ofdmProcessor	= new ofdmProcessor   (inputDevice,
	                                               dabMode,
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
              if (!filename. endsWith (".wav", Qt::CaseInsensitive) &&
	          !filename. endsWith (".eti", Qt::CaseInsensitive))
                 filename. append (".eti");
	      eti_file	= fopen (filename. toLatin1 (). data (), "wb");
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


