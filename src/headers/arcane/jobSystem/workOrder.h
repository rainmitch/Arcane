#ifndef ARCANE_WORKORDER_H
#define ARCANE_WORKORDER_H

#include <cstdint>

#define WORK_ORDER_SIZE 65355

namespace arcane
{
	class WorkOrder
	{
	public:
		WorkOrder ();

		uint32_t associatedJobs;
		uint32_t id;
	};
}

#endif //ARCANE_WORKORDER_H
