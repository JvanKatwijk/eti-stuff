
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of eti-stuff
 *
 *    eti-stuff is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti-stuff is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti-stuff; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__VITERBI_HANDLER__
#define	__VITERBI_HANDLER__

#include	<stdint.h>

class	viterbiHandler {

public:
		viterbiHandler	(int);
		~viterbiHandler	(void);
	void	deconvolve	(int16_t *, uint8_t *);
private:
	int     costTable [16];
	void	computeCostTable (int16_t,  int16_t, int16_t, int16_t);
	uint8_t	bitFor		(int, int, int);
	int	blockLength;
	int	*stateSequence;
	int	**transCosts;
	int	**history;
};

#endif

	
