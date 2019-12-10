#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the eti-cmdline handler
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
 *
 * 	Default (void) implementation of
 * 	virtual deviceHandler class
 */
#include	"device-handler.h"

	deviceHandler::deviceHandler (void) {
	lastFrequency	= 100000;
}

	deviceHandler::~deviceHandler (void) {
}

bool	deviceHandler::restartReader	(int32_t frequency) {
	(void)frequency;
	return true;
}

void	deviceHandler::stopReader	(void) {
}

int32_t	deviceHandler::getSamples	(DSPCOMPLEX *v, int32_t amount) {
	(void)v; 
	(void)amount; 
	return 0;
}

int32_t	deviceHandler::Samples		(void) {
	return 0;
}

void	deviceHandler::resetBuffer	(void) {
}

