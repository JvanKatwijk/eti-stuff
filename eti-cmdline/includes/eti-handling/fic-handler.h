#
/*
 *    Copyright (C) 2016 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti-cmdline program
 *
 *    eti-cmdline is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti-cmdline is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti-cmdline; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * 	FIC data
 */
#pragma once

#include	<stdio.h>
#include	<stdint.h>
#include	"viterbi-spiral.h"
#include	"callback-types.h"
#include	"fib-processor.h"

class	dabParams;

class ficHandler:public viterbiSpiral {
public:
			ficHandler		(dabParams	*,
	                                         callbacks	*,
	                                         void	    *);
			~ficHandler		(void);
	void		process_ficBlock	(int16_t *, uint8_t *, bool *);
	void		clearEnsemble		(void);
	int16_t		get_ficRatio		(void);
	void		get_channelInfo		(channel_data *, int);
	void		get_CIFCount		(int16_t *, int16_t *);
private:
	fibquality_t	set_fibQuality;
	void		*userData;
	fib_processor	fibProcessor;
	void		process_ficInput	(int16_t *,
	                                         int16_t, uint8_t *, bool *);
	uint8_t		bitBuffer_out	[768];
	int16_t		ofdm_input	[2304];
	bool		punctureTable	[3072 + 24];
	uint8_t		PRBS 		[768];
	int16_t		index;
	int16_t		BitsperBlock;
	int16_t		ficno;
	int16_t		ficBlocks;
	int16_t		ficMissed;
	int16_t		ficRatio;
	int16_t		ficErrors;
	int16_t		ficSuccess;
};

