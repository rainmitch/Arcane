
#ifndef ARCANE_OBJLOADER_H
#define ARCANE_OBJLOADER_H

#include <fstream>

#include <arcane//mesh.h>

namespace arcane
{
	Mesh loadOBJ (const std::string &path);
}
#endif //ARCANE_OBJLOADER_H
