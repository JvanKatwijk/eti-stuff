#
/*
 *    Copyright (C) 2016. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  eti-frontend of SDR-J.
 *    eti frontend is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti frontend is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti frontend; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<QDir>
#include	<unistd.h>
#include	"dab-constants.h"
#include	"radio.h"

QString	fullPathfor (QString v) {
QString	fileName;

	if (v == QString ("")) 
	   return QString ("/tmp/xxx");

	if (v. at (0) == QChar ('/')) 		// full path specified
	   return v;

	fileName = QDir::homePath ();
	fileName. append ("/");
	fileName. append (v);
	fileName = QDir::toNativeSeparators (fileName);

	if (!fileName. endsWith (".ini"))
	   fileName. append (".ini");

	return fileName;
}

bool	fileExists (char *v) {
FILE *f;

	f = fopen (v, "r");
	if (f == NULL)
	   return false;
	fclose (f);
	return true;
}

#define	DEFAULT_INI	".dab2eti.ini"

int	main (int argc, char **argv) {
/*
 *	The default values
 */
QString		initFileName;
QString		outputFileName;
RadioInterface	*MyRadioInterface;
int32_t		opt;
uint8_t		syncMethod	= 1;
QSettings	*dabSettings;		// ini file
uint8_t		dabMode		= 127;	// illegal value
QString		dabDevice	= QString ("");
QString		dabBand		= QString ("");
FILE		*outputFile	= NULL;	// probably overruled


	while ((opt = getopt (argc, argv, "i:D:S:M:B:O:")) != -1) {
	   switch (opt) {
	      case 'i':
	         initFileName = fullPathfor (optarg);
	         break;

	      case 'S':
	         syncMethod	= atoi (optarg);
	         break;

	      case 'M':
	         dabMode	= atoi (optarg);
	         if (!(dabMode == 1) || (dabMode == 2) || (dabMode == 4))
	            dabMode = 1; 
	         break;

	      case 'B':
	         dabBand 	= optarg;
	         break;

	      case 'o':
	      case 'O':
	         if (optarg [0] == '-')
	            outputFile = stdout;
	         else {
	            outputFile = fopen (optarg, "wb");
	            if (outputFile == NULL) {
	               fprintf (stderr, "opening %s failed, defaulting to stdout\n", optarg);
	               outputFile = stdin;
	            }
	         }
	         break;
	      default:
	         break;
	   }
	}

	dabSettings =  new QSettings (initFileName, QSettings::IniFormat);

	if (dabMode == 127)
	   dabMode = dabSettings -> value ("dabMode", 1). toInt ();
	if (dabBand == QString (""))
	   dabBand = dabSettings -> value ("band", "BAND III"). toString ();
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	QApplication a (argc, argv);
#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif
//	save the values for the new defaults
	dabSettings	-> setValue ("dabMode",	dabMode);
	dabSettings	-> setValue ("band",	dabBand);
	dabSettings	-> sync ();
	MyRadioInterface = new RadioInterface (dabSettings, 
	                                       dabMode,
	                                       dabBand,
	                                       syncMethod,
	                                       outputFile);
	MyRadioInterface -> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	MyRadioInterface	-> ~RadioInterface ();
	dabSettings		-> sync ();
	exit (1);
}

