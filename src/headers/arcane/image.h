#ifndef ARCANE_IMAGE_H
#define ARCANE_IMAGE_H

#include <vector>
#include <string>

namespace arcane
{
	typedef struct
	{
		std::vector<unsigned char> data;
		unsigned int width;
		unsigned int height;
	} Image;
	Image loadPNGTexture (const std::string &path);
}

#endif //ARCANE_IMAGE_H
