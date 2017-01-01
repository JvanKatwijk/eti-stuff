#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<QDir>
#include	<unistd.h>
#include	"dab-constants.h"
#include	"gui.h"

void	fullPathfor (const char *v, char *out) {
int16_t	i;
QString	homeDir;

	if (v == NULL) {
	   sprintf (out, "%s", "/tmp/xxx");
	   return;	// should not happen
	}

	if (v [0] == '/') {		// full path specified
	   sprintf (out, "%s", v);
	   return;
	}

	homeDir = QDir::homePath ();
	homeDir. append ("/");
	homeDir. append (v);
	homeDir	= QDir::toNativeSeparators (homeDir);
	sprintf (out, "%s", homeDir. toLatin1 (). data ());
	fprintf (stderr, "ini file = %s\n", out);

	for (i = 0; out [i] != 0; i ++);
	if (out [i - 4] != '.' ||
	    out [i - 3] != 'i' ||
	    out [i - 2] != 'n' ||
	    out [i - 1] != 'i') {
	    out [i] = '.';
	    out [i + 1] = 'i';
	    out [i + 2] = 'n';
	    out [i + 3] = 'i';
	    out [i + 4] = 0;
	}
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
char		defaultInit [512];
char		outputFilename [512];
RadioInterface	*MyRadioInterface;
int32_t		opt;
uint8_t		syncMethod	= 1;
QSettings	*dabSettings;		// ini file
uint8_t		dabMode		= 127;	// illegal value
QString		dabDevice	= QString ("");
QString		dabBand		= QString ("");
FILE		*outputFile	= NULL;

	fullPathfor (DEFAULT_INI, defaultInit);

	while ((opt = getopt (argc, argv, "i:D:S:M:B:O:")) != -1) {
	   switch (opt) {
	      case 'i':
	         fullPathfor (optarg, defaultInit);
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
	            fullPathfor (optarg, outputFilename);
	            outputFile = fopen (outputFilename, "w");
	         }
	         break;
	      default:
	         break;
	   }
	}
	dabSettings =  new QSettings (defaultInit, QSettings::IniFormat);


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

