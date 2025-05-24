#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti library
 *
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
 */
#include	"semaphore.h"

	semaphore::semaphore	(int count) {
	this	-> count = count;
}

	semaphore::~semaphore	() {}

void	semaphore::release	() {
	std::unique_lock<std::mutex> lock(mtx);
	count++;
	cv.notify_one();
}

void	semaphore::wait		() {
	std::unique_lock<std::mutex> lock(mtx);
	while (count == 0) {
	   cv.wait(lock);
	}
        count--;
}
