
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
