#ifndef ARCANE_LIGHT_H
#define ARCANE_LIGHT_H

#include <arcane/glm.h>

#define MAX_LIGHTS 64

namespace arcane
{
	class Light
	{
	public:
		Light (const vec3 &position=vec3(0, 0, 0), const vec3 &color=vec3(1, 1, 1), const float intensity=1);
		vec3 position;
		vec3 color;
		float intensity;
	};
}

#endif //ARCANE_LIGHT_H
