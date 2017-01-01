#
/*
 *    Copyright (C)  2010, 2011, 2012
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
 */

#ifndef _GUI
#define _GUI

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QStringListModel>
#include	<QComboBox>
#include	<QLabel>
#include	<sndfile.h>
#include	"ui_gui.h"
#include	<QTimer>
#include	"ringbuffer.h"
#include	"fib-processor.h"

class	QSettings;
class	audioBase;

class	etiController;

/*
 *	GThe main gui object. It inherits from
 *	QDialog and the generated form
 */
class RadioInterface: public QMainWindow,
		      private Ui_dab_rpi {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 FILE		*,
	                                 QWidget *parent = NULL);
		~RadioInterface		();

private:
	QSettings	*dabSettings;
	void		setModeParameters	(uint8_t);
	void		clear_showElements	(void);
	DabParams	dabModeParameters;
	bool		running;
	fib_processor	*my_fibHandler;
	etiController	*my_etiController;
	audioBase	*soundOut;
	RingBuffer<int16_t>	*audioBuffer;
	RingBuffer<fibElement>	*fibBuffer;
const	char		*get_programm_type_string (uint8_t);
const	char		*get_programm_language_string (uint8_t);
	QLabel		*pictureLabel;
	void		init_your_gui		(void);
	bool		audioDumping;
	SNDFILE		*audiofilePointer;
	QStringListModel	ensemble;
	QStringList	Services;
	QString		ensembleLabel;
	QTimer		*displayTimer;
	int32_t		numberofSeconds;
	void		resetSelector		(void);
	int16_t		fibBlocks;
	int16_t		fibSuccess;
public slots:
	void		clearEnsemble		(void);
	void		addtoEnsemble		(const QString &);
	void		nameofEnsemble		(int, const QString &);
	void		show_successRate	(int);
	void		show_ipErrors		(int);
	void		showLabel		(QString);
	void		showMOT			(QByteArray, int);
	void		changeinConfiguration	(void);
	void		newAudio		(int);
	void		handle_fibData		(void);
//
	void	show_mscErrors		(int);
	void	setStereo		(bool);
private slots:
//
//	Somehow, these must be connected to the GUI
//	We assume that any GUI will need these three:
	void	setStart		(void);
	void	TerminateProcess	(void);
	void	updateTimeDisplay	(void);
	void	selectService		(QModelIndex);
	void	set_audioDump		(void);
};

#endif

