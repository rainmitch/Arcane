
#include <arcane/camera.h>

namespace arcane
{
	Camera::Camera (const vec3 &position, const vec3 &rotation)
	{
		transform = Transform (position, rotation);
	}

	mat4 Camera::getViewMatrix ()
	{
		vec4 offset = vec4 (0, 0, 1, 1);

		mat4 m = transform.rotationMat4 ();

		vec3 mOffset = vec3 (offset * mat3 (m));

		mat4 cameraMatrix = lookAt (transform.position,transform.position + mOffset, vec3 (0, 1,0));

		return cameraMatrix;
	}
}
