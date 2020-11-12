
#include <arcane/light.h>

namespace arcane
{
	Light::Light (const vec3 &position, const vec3 &color, const float intensity)
	{
		this->position = position;
		this->color = color;
		this->intensity = intensity;
	}
}