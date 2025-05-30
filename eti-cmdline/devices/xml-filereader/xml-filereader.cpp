#
/*
 *    Copyright (C) 2013 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of eti-cmdline
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
#include	"xml-filereader.h"
#include	<cstdio>
#include	<unistd.h>
#include	<cstdlib>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<ctime>

#include	"xml-descriptor.h"
#include	"xml-reader.h"

#include	"dab-constants.h"

#define	INPUT_FRAMEBUFFERSIZE	8 * 32768
//
//
	xml_fileReader::xml_fileReader (std::string fileName,
	                                bool	continue_on_eof,
	                                inputstopped_t inputStopped):
	                                  _I_Buffer (INPUT_FRAMEBUFFERSIZE) {
	this -> fileName	= fileName;
	fprintf (stderr, "trying to open %s\n", fileName. c_str ());
	theFile	= fopen (fileName.c_str (), "rb");
	if (theFile == nullptr) {
	   fprintf (stderr, "file %s cannot open\n",
	                                   fileName. c_str ());
	   perror ("file ?");
	   throw (31);
	}
	
	bool	ok	= false;
	theDescriptor	= new xmlDescriptor (theFile, &ok);
	if (!ok) {
	   fprintf (stderr, "%s probably not an xml file\n",
	                               fileName. c_str ());
	   throw (32);
	}

	fprintf (stderr, "nrElements = %ld\n",
	             theDescriptor -> blockList [0].nrElements);
	theReader	= nullptr;
	this	-> continue_on_eof	= continue_on_eof;
}

	xml_fileReader::~xml_fileReader	() {
	if (theReader != nullptr)
	   delete theReader;

	if (theFile != nullptr)
	   fclose (theFile);

	delete	theDescriptor;
}

bool	xml_fileReader::restartReader (int32_t freq) {
	(void)freq;
	if (theReader != nullptr)
	   return true;
	theReader	= new xml_Reader (theFile,
	                                  theDescriptor,
	                                  5000,
	                                  &_I_Buffer,
	                                  continue_on_eof);
	return true;
}

void	xml_fileReader::stopReader () {
	if (theReader != nullptr)
	   delete theReader;
	theReader = nullptr;
}

//	size is in "samples"
int32_t	xml_fileReader::getSamples	(std::complex<float> *V,
	                                 int32_t size) {

	if (theFile == nullptr)		// should not happen
	   return 0;

	while ((int32_t)(_I_Buffer. GetRingBufferReadAvailable()) < size)
	   special_usleep (1000);

	return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	xml_fileReader::Samples	() {
	if (theFile == nullptr)
	   return 0;
	return _I_Buffer. GetRingBufferReadAvailable();
}

