
#include <arcane/jobSystem/semaphore.h>

namespace arcane
{
	namespace JobSystem
	{
		void Semaphore::notify ()
		{
			std::lock_guard<decltype (mutex)> lock (mutex);
			++count;
			condition.notify_one ();
		}

		void Semaphore::wait ()
		{
			std::unique_lock<decltype (mutex)> lock (mutex);
			while (!count) // Handle spurious wake-ups.
				condition.wait (lock);
			--count;
		}

		bool Semaphore::try_wait ()
		{
			std::lock_guard<decltype (mutex)> lock (mutex);
			if (count)
			{
				--count;
				return true;
			}
			return false;
		}
	}
}