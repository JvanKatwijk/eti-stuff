#
/*
 *    Copyright (C)  2016 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of eti-backend
 *
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
 */

#ifndef _RADIO_
#define _RADIO_

#include	"dab-constants.h"
#include	<QMainWindow>
#include	<QStringList>
#include	<QStringListModel>
#include	<QComboBox>
#include	<QLabel>
#include	<QTimer>
#include	<sndfile.h>
#include	"ui_eti-backend.h"
#include	"ui_technical_data.h"
#include	"ringbuffer.h"
#include	"fib-processor.h"

class	QSettings;
class	audioBase;

class	etiController;

/*
 *	The main gui object. It inherits from
 *	QDialog and the generated form
 */
class RadioInterface: public QMainWindow,
		      private Ui_eti_backend {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 FILE		*,
	                                 QWidget *parent = NULL);
		~RadioInterface		();

private:
	Ui_technical_data	techData;
	QFrame		dataDisplay;
	bool		show_data;
private slots:
	void		toggle_show_data	(void);
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
	bool		saveSlide;
	void		init_your_gui		(void);
	bool		audioDumping;
	SNDFILE		*audiofilePointer;
	QStringListModel	ensemble;
	QStringList	Services;
	QString		ensembleLabel;
	QTimer		*displayTimer;
	int32_t		numberofSeconds;
	void		resetSelector		(void);
	int16_t		ficBlocks;
	int16_t		ficSuccess;
public slots:
	void		clearEnsemble		(void);
	void		addtoEnsemble		(const QString &);
	void		nameofEnsemble		(int, const QString &);
	void            show_frameErrors        (int);
        void            show_rsErrors           (int);
        void            show_aacErrors          (int);
        void            show_ficSuccess         (bool);

	void		show_ipErrors		(int);
	void		showLabel		(QString);
	void		showMOT			(QByteArray, int, QString);
	void		changeinConfiguration	(void);
	void		newAudio		(int);
	void		handle_fibData		(void);
//
	void		show_mscErrors		(int);
	void		setStereo		(bool);
	void		show_motHandling	(bool);
	void		set_streamSelector	(int);
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

