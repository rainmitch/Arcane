
#include <arcane/jobSystem/worker.h>

#include <iostream>

#include <arcane/jobSystem/workOrder.h>

namespace arcane
{
	namespace JobSystem
	{
		extern bool jobSystemRunning;

		Worker::Worker (const size_t &id)
		{
			m_id = id;
			jobQueue = new CircularQueue<Job, JOB_QUEUE_SIZE> ();
			m_thread = new std::thread ([this]() {this->doWork ();});
			workOrderJobsCompleted = new uint32_t[WORK_ORDER_SIZE];
			for (size_t i = 0; i < WORK_ORDER_SIZE; i++)
			{
				workOrderJobsCompleted[i] = 0;
			}
		}

		Worker::~Worker ()
		{
			m_semaphore.notify ();
			if (m_thread != nullptr)
			{
				if (m_thread->joinable ())
					m_thread->join ();
				delete m_thread;
			}
			delete jobQueue;
		}

		void Worker::doWork ()
		{
			while (jobSystemRunning)
			{
				Job *job = jobQueue->peek ();
				if (job->ready && !job->hasRun)
				{
					job->func ();
					job->hasRun = true;
					workOrderJobsCompleted[job->workOrderID]++;

					jobQueue->next ();
				}
				else
				{
					m_semaphore.wait ();
				}
			}
		}

		size_t Worker::id () const
		{
			return m_id;
		}

		Job *Worker::getNextFreeJob ()
		{
			return jobQueue->getNextFreeSlot ();
		}


		uint32_t Worker::jobsCompletedFromWorkOrder (const uint32_t &id)
		{
			return workOrderJobsCompleted[id];
		}

		void Worker::wakeUp ()
		{
			m_semaphore.notify ();
		}

		void Worker::terminate ()
		{
			m_semaphore.notify ();
			if (m_thread->joinable ())
				m_thread->join ();
			delete m_thread;
			delete jobQueue;
			m_thread = nullptr;
		}
	}
}