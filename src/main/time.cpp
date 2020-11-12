
#include <arcane/time.h>

namespace arcane
{
	namespace Time
	{
		float internalDelta = 1.0f/60;

		float delta ()
		{
			return internalDelta;
		}
	}
}