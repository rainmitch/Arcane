
#include <arcane/transform.h>
#include <arcane/time.h>

namespace arcane
{

	Transform::Transform (const vec3 &position, const vec3 &rotation)
	{
		this->position = position;
		this->rotation = rotation;
	}

	void Transform::move (const vec3 &offset)
	{
		vec4 o = vec4 (offset, 1) * rotationMat4 ();
		position += vec3 (o);
	}

	mat4 Transform::rotationMat4 ()
	{
		mat4 m (1);

		m = rotate (m, rotation.x * 0.0174533f, vec3 (1, 0, 0));
		m = rotate (m, rotation.y * 0.0174533f, vec3 (0, 1, 0));
		m = rotate (m, rotation.z * 0.0174533f, vec3 (0, 0, 1));

		return m;
	}
}