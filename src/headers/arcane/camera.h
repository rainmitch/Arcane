#ifndef ARCANE_CAMERA_H
#define ARCANE_CAMERA_H

#include <arcane/transform.h>

namespace arcane
{
	class Camera
	{
	public:
		explicit Camera (const vec3 &position=vec3(0, 0, 0), const vec3 &rotation=vec3(0,0,0));

		mat4 getViewMatrix ();
		Transform transform;
	};
}


#endif //ARCANE_CAMERA_H
