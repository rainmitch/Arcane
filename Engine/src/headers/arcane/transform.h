#ifndef ARCANE_TRANSFORM_H
#define ARCANE_TRANSFORM_H

#include <arcane/glm.h>

namespace arcane
{
	class Transform
	{
	public:
		explicit Transform (const vec3 &position=vec3(0, 0, 0), const vec3 &rotation=vec3(0,0,0));

		void move (const vec3 &offset);

		mat4 rotationMat4 ();


		vec3 position;
		vec3 rotation;
	};
}

#endif //ARCANE_TRANSFORM_H
