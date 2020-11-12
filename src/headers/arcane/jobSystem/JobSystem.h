
#ifndef ARCANE_JOBSYSTEM_H
#define ARCANE_JOBSYSTEM_H

#include <thread>
#include <vector>
#include <iostream>
#include <functional>
#include <atomic>
#include <map>

#include <arcane/jobSystem/circularQueue.h>
#include <arcane/jobSystem/workOrder.h>
#include <arcane/jobSystem/job.h>


namespace arcane
{
	namespace JobSystem
	{
		void init ();

		size_t threadCount ();

		WorkOrder *createWorkOrder ();
		bool workOrderCompleted (WorkOrder *workOrder);

		Job *createJob (WorkOrder *workOrder, const std::function<void ()> &func);


		void terminate ();
	}
}

#endif //ARCANE_JOBSYSTEM_H
