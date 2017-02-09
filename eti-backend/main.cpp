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

#define	DEFAULT_INI	".dab-backend.ini"

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

	while ((opt = getopt (argc, argv, "LABCi:F:f:")) != -1) {
	   switch (opt) {
	      case 'i':
	         initFileName = fullPathfor (optarg);
	         break;
	      case 'f':
	      case 'F':
	         inputName	= fullPathfor (optarg);
	         inputFile	= fopen (inputName. toLatin1 (). data (), "r");
	         if (inputFile == NULL)
	            fprintf (stderr, "opening %s failed\n",
	                                   inputName. toLatin1 (). data ());
	         break;
	   }
	}

	if (initFileName == QString (""))
	   initFileName = fullPathfor (DEFAULT_INI);
	if (inputFile == NULL)
	   inputFile	= stdin;

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

