#
/*
 *    Copyright (C)  2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti-frontend
 *    eti-frontend is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti-frontend is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti-frontend; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __RADIO__
#define __RADIO__

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QStringListModel>
#include	<QComboBox>
#include	<QLabel>
#include	"ui_gui.h"
#include	<QTimer>
#include	"ofdm-processor.h"
#include	"ringbuffer.h"
#include	"band-handler.h"

class	QSettings;
class	virtualInput;
class	ficHandler;
class	etiGenerator;


/*
 */
class RadioInterface: public QMainWindow,
		      private Ui_dab2eti {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 uint8_t	dabMode,
	                                 QString	dabBand,
	                                 uint8_t	freqsyncMethod,
	                                 FILE		*eti_file,
	                                 QWidget *parent = NULL);
		~RadioInterface		();

private:
	QSettings	*dabSettings;
	bool		autoStart;
	int16_t		threshold;
	uint8_t		freqsyncMethod;
	FILE		*eti_file;
	int32_t		vfoFrequency;
	void		clear_showElements	(void);
	uint8_t		isSynced;
	uint8_t		dabBand;
	bandHandler	theBand;
	uint8_t		dabMode;
	bool		running;
	bool		eti_running;
	virtualInput	*inputDevice;
	ofdmProcessor	*my_ofdmProcessor;
	ficHandler	*my_ficHandler;
	etiGenerator	*my_etiGenerator;
	bool		autoCorrector;
	void		init_your_gui		(void);
	void		dumpControlState	(QSettings *);
	bool		sourceDumping;
	SNDFILE		*dumpfilePointer;
	QStringListModel	ensemble;
	QStringList	Services;
	QString		ensembleLabel;
	QTimer		*displayTimer;
	int32_t		numberofSeconds;
	void		resetSelector		(void);
	int16_t		ficBlocks;
	int16_t		ficSuccess;
public slots:
	void	set_fineCorrectorDisplay	(int);
	void	set_coarseCorrectorDisplay	(int);
	void	clearEnsemble		(void);
	void	addtoEnsemble		(const QString &);
	void	nameofEnsemble		(int, const QString &);
	void	show_ficCRC		(bool);
	void	show_snr		(int);
	void	setSynced		(char);
	void	changeinConfiguration	(void);
//
private slots:
//
//	Somehow, these must be connected to the GUI
//	We assume that any GUI will need these three:
	void	setStart		(void);
	void	TerminateProcess	(void);
	void	set_channelSelect	(QString);
	void	updateTimeDisplay	(void);
	void	autoCorrector_on	(void);
	void	setDevice		(QString);
	void	set_dumping		(void);
	void	set_etiControl		(void);
};

#endif

