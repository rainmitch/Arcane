
#include <arcane/image.h>

#include <arcane/lodepng.h>
#include <stdexcept>

namespace arcane
{
	Image loadPNGTexture (const std::string &path)
	{
		std::vector<unsigned char> image;
		unsigned int width, height;

		unsigned int error = lodepng::decode (image, width, height, path.c_str ());

		if (error)
		{
			throw std::runtime_error ("png decoder error!\nreason: " + std::string (lodepng_error_text(error)));
		}

		return {image, width, height};
	}
}