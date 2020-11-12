
#ifndef ARCANE_JOB_H
#define ARCANE_JOB_H

#include <functional>

namespace arcane
{

	class Job
	{
	public:
		Job ();

		bool ready;
		bool hasRun;
		std::function<void ()> func;
		uint32_t workOrderID;
	};
}

#endif //ARCANE_JOB_H
