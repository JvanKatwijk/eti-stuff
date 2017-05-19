#
/*
 *    Copyright (C) 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti-cmdline program.
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
#include	"eti-class.h"

//      Initialization:
//	etiClass is basically nothing more than a
//	combination of ofdmProcessor, the entrypoint for all
//	ofdm handling, and etiGenerator, the entrypoint for
//	interpreting the data and generating output

	etiClass::etiClass	(uint8_t	theMode,
	                         deviceHandler   *inputDevice,
//      The callbacks
	                         syncsignal_t    syncsignalHandler,
	                         snrsignal_t     snrsignalHandler,
	                         ensemblename_t  ensemblenameHandler,
	                         programname_t   programnameHandler,
	                         fibquality_t    fibqualityHandler,
	                         etiwriter_t     etiwriterHandler,
//
	                         void		*userData
	                        ): my_etiGenerator (theMode,
	                                            userData,
	                                            ensemblenameHandler,
	                                            programnameHandler,
	                                            fibqualityHandler,
	                                            etiwriterHandler),
	                           my_ofdmProcessor (inputDevice,
	                                             theMode,
	                                             userData,
	                                             syncsignalHandler,
	                                             snrsignalHandler,
	                                             &my_etiGenerator,
	                                             3, 2) {
}

	etiClass::~etiClass		(void) {
	my_ofdmProcessor. stop ();
	my_etiGenerator.  stop ();
}

void	etiClass::stop		(void) {
        my_ofdmProcessor. stop ();
        my_etiGenerator.  stop ();
}

void	etiClass::start_ofdmProcessing	(void) {
	my_ofdmProcessor. start ();
}

void	etiClass::start_etiProcessing	(void) {
	my_etiGenerator. startProcessing ();
}

void	etiClass::reset			(void) {
	my_ofdmProcessor. stop ();
	my_etiGenerator.  reset ();
}

void	etiClass::set_syncReached         (void) {
	my_ofdmProcessor. syncReached ();
}

