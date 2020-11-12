#ifndef ARCANE_SEMAPHORE_H
#define ARCANE_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace arcane
{
	namespace JobSystem
	{
		class Semaphore
		{
		public:
			void notify ();

			void wait ();

			bool try_wait ();

		private:
			std::mutex mutex;
			std::condition_variable condition;
			unsigned long count = 0; // Initialized as locked.
		};
	}
}

#endif //ARCANE_SEMAPHORE_H
