
#include "internal.h"

#include <GL/glew.h>
#include <vector>
#include <arcane/mesh.h>
#include <map>

#include "batchingObject.h"

namespace arcane
{
	namespace OpenGL43
	{
		extern std::map<std::string, BatchingObject> batchedObjects;
	}
}