
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
 *    Copyright (C) 2016
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti library
 *    eti library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	eti generator
 */

#ifndef	__ETI_GENERATOR__
#define	__ETI_GENERATOR__
#include	<stdio.h>
#include	<stdint.h>
#include	<stdio.h>
#include	<thread>
#include	"semaphore.h"
#include	<vector>
#include	<atomic>
#include	"callback-types.h"
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"fic-handler.h"
#include	"dab-params.h"
#include	"protection.h"

class	RadioInterface;
class	ensembleHandler;

typedef struct {
	int16_t		blkno;
	int16_t 	data [3072];
} bufferElement;

class	parameter;

//
//	to build a simple cache for the protection handlers
typedef	struct {
	bool    uepFlag;
        int     bitRate;
        int     protLevel;
	uint8_t	*dispersionVector;
        protection *theDeconvolver;
} protDesc;

class etiGenerator {
public:
		etiGenerator	(uint8_t,
	                         int,
	                         callbacks	*,
	                         void		*);
		~etiGenerator	(void);
	void	newFrame	(void);
	void	processBlock	(int16_t *fbits, int16_t blkno);
	void	stop		(void);
	void	startProcessing	(void);
	void	reset		(void);
private:
	semaphore	theLocker;
	void		*userData;
	etiwriter_t	etiWriter;
	std::thread	threadHandle;
	std::atomic<bool>	running;
	void		start	(void);
	void		run	(void);
	dabParams	params;
	RingBuffer<bufferElement> *dataBuffer;
	FILE		*outputFile;
	std::vector<int16_t> fibInput;
	int16_t		index_Out;
	int16_t		CIFCount_hi;
	int16_t		CIFCount_lo;
	std::atomic <int16_t>	amount;
	int16_t		BitsperBlock;
	int16_t		numberofblocksperCIF;
	ficHandler	my_ficHandler;
	std::atomic<bool>	processing;
	int16_t		expected_block;
	int32_t		init_eti		(uint8_t *,
	                                         int16_t, int16_t, int16_t);
	int32_t		process_CIF		(int16_t *,
	                                         uint8_t *, int32_t);
//	void		process_subCh		(parameter	*);

	void		postProcess		(uint8_t *, int32_t);
};

#endif


