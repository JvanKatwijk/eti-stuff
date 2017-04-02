#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti backend
 *    eti-backend is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti-backend is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti-backend; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<QDir>
#include	<stdio.h>
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

#define	DEFAULT_INI	".eti-backend.ini"

int	main (int argc, char **argv) {
/*
 *	The default values
 */
QSettings	*ISettings;		// ini file
QString		initFileName = QString ("");
QString		inputName;
RadioInterface	*MyRadioInterface;
int32_t		opt;
FILE		*inputFile	= NULL;

	while ((opt = getopt (argc, argv, "i:s:S:")) != -1) {
	   switch (opt) {
	      case 'i':
	         initFileName = fullPathfor (optarg);
	         break;

	      case 's':
	      case 'S':
	         if (optarg[0]  == '-')
	            inputFile = stdin;
	         else {
	            fprintf (stderr, "else part\n");
	            inputFile = fopen (optarg, "rb");
	            if (inputFile == NULL) {
	               fprintf (stderr, "could not open %s, defaulting to stdin\n",
	                                          optarg);
	               inputFile = stdin;
	            }
	         }
	fprintf (stderr, "end of -s\n");
	         break;
	   }
	}

	if (initFileName == QString (""))
	   initFileName = fullPathfor (DEFAULT_INI);

	ISettings	= new QSettings (initFileName, QSettings::IniFormat);
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	QApplication a (argc, argv);
	MyRadioInterface = new RadioInterface (ISettings, inputFile);
#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif
	MyRadioInterface -> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	MyRadioInterface	-> ~RadioInterface ();
	ISettings		-> ~QSettings ();
	exit (1);
}

