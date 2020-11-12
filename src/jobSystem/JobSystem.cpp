
#include <arcane/jobSystem/JobSystem.h>

#include <list>
#include <atomic>

#include <mutex>
#include <thread>

#include <arcane/jobSystem/worker.h>

namespace arcane
{

	namespace JobSystem
	{
		size_t internalThreadCount;
		Worker **workers;
		WorkOrder *workOrders;
		bool jobSystemRunning = true;

		size_t workOrdersIndex;
		size_t workersIndex;

		void init ()
		{
			internalThreadCount = 12;//std::thread::hardware_concurrency ();
			workOrdersIndex = 0;
			workersIndex = 0;
			if (internalThreadCount == 0)
			{
				internalThreadCount = 4;
				//todo: find alternative method of finding hardware thread number
			}

			workers = new Worker *[internalThreadCount];
			workOrders = new WorkOrder[WORK_ORDER_SIZE];
			for (size_t i = 0; i < WORK_ORDER_SIZE; i++)
			{
				workOrders[i] = WorkOrder ();
				workOrders[i].id = i;
			}

			for (size_t i = 0; i < internalThreadCount; i++)
			{
				workers[i] = new Worker (i);
			}
		}

		size_t threadCount ()
		{
			return internalThreadCount;
		}

		WorkOrder *createWorkOrder ()
		{
			WorkOrder *workOrder = &workOrders[workOrdersIndex];
			workOrdersIndex = (workOrdersIndex + 1) % WORK_ORDER_SIZE;

			return workOrder;
		}

		bool workOrderCompleted (WorkOrder *workOrder)
		{
			uint32_t total = 0;

			for (size_t i = 0; i < internalThreadCount; i++)
			{
				total += workers[i]->jobsCompletedFromWorkOrder (workOrder->id);
			}

			return total == workOrders[workOrder->id].associatedJobs;
		}


		Job *createJob (WorkOrder *workOrder, const std::function<void ()> &func)
		{
			Job *job = workers[workersIndex]->getNextFreeJob ();

			job->workOrderID = workOrder->id;
			job->hasRun = false;
			job->ready = true;
			job->func = func;
			workOrders[workOrder->id].associatedJobs++;

			workers[workersIndex]->wakeUp ();
			workersIndex = (workersIndex + 1) % internalThreadCount;

			return job;
		}

		void terminate ()
		{
			jobSystemRunning = false;

			for (size_t i = 0; i < internalThreadCount; i++)
			{
				workers[i]->terminate ();
			}
		}
	}
}