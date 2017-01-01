#
/*
 * ## Copyright
 *
 * dabtools is written by Dave Chapman <dave@dchapman.com> 
 *
 * Large parts of the code are copied verbatim (or with trivial
 * modifications) from David Crawley's OpenDAB and hence retain his
 * copyright.
 *
 *	Parts of this software are copied verbatim (or with trivial
 *	Modifications) from David Chapman's dabtools and hence retain
 *	his copyright. In particular, the crc, descramble and init_eti
 *	functions are - apart from naming - a verbatim copy. Thanks
 *	for the nice work
 *
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J (JSDR).
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
 */
#
/*
 * 	eti generator
 */
#
#ifndef	ETI_GENERATOR
#define	ETI_GENERATOR
#include	<QThread>
#include	<stdio.h>
#include	<stdint.h>
#include	"deconvolve.h"
#include	<stdio.h>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"fic-handler.h"
class	RadioInterface;

typedef struct {
	int16_t		blkno;
	int16_t 	data [3072];
} bufferElement;

class etiGenerator: public QThread {
public:
		etiGenerator	(RadioInterface *,
	                         DabParams	*);
		~etiGenerator	(void);
	void	newFrame	(void);
	void	processBlock	(int16_t *fbits, int16_t blkno);
	void	startProcessing	(FILE *);
	void	stopProcessing	(void);
protected:
	void	run		(void);
private:
	RadioInterface	*myRadioInterface;
	DabParams	*params;
	RingBuffer<bufferElement> *dataBuffer;
	FILE		*outputFile;
	bool		running;
	int16_t		*fibInput;
	int16_t		index_Out;
	int16_t		CIFCount_hi;
	int16_t		CIFCount_lo;
	int16_t		amount;
	int16_t		BitsperBlock;
	int16_t		numberofblocksperCIF;
	ficHandler	my_ficHandler;
	bool		processing;
	int16_t		expected_block;
	void		time_deinterleave	(int16_t *);
	void		disperse		(uint8_t *, int16_t);
	void		dab_descramble_bytes	(uint8_t *, int32_t); 
	int32_t		init_eti		(uint8_t *,
	                                         int16_t, int16_t, int16_t);
	int32_t		process_CIF		(int16_t *,
	                                         uint8_t *, int32_t);
	void		postProcess		(uint8_t *, int32_t);
};

#endif


