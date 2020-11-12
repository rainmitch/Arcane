#ifndef ARCANE_WORKER_H
#define ARCANE_WORKER_H

#include <cstdint>
#include <thread>
#include <vector>

#include <arcane/jobSystem/semaphore.h>
#include <arcane/jobSystem/job.h>
#include <arcane/jobSystem/circularQueue.h>


#define JOB_QUEUE_SIZE 8192

namespace arcane
{
	namespace JobSystem
	{
		class Worker
		{
		public:
			explicit Worker (const size_t &id);
			~Worker ();

			void doWork ();
			size_t id () const;

			Job *getNextFreeJob ();

			uint32_t jobsCompletedFromWorkOrder (const uint32_t &id);

			void wakeUp ();
			void terminate ();

		private:
			Semaphore m_semaphore;
			size_t m_id;
			std::thread *m_thread;
			CircularQueue<Job, JOB_QUEUE_SIZE> *jobQueue;
			uint32_t *workOrderJobsCompleted;
		};
	}
}


#endif //ARCANE_WORKER_H
