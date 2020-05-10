

#ifndef	__SEMAPHORE__
#define	__SEMAPHORE__

#include	<mutex>
#include	<condition_variable>


class	semaphore {
public:
		semaphore	(int count = 0);
	 	~semaphore	();
	void	release		();
	void	wait		();
private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};

#endif
