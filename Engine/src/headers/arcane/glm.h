//
// Created by Rain on 9/10/2020.
//

#ifndef ARCANE_GLM_H
#define ARCANE_GLM_H

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>


#include <iostream>

using namespace glm;

inline std::ostream &operator<< (std::ostream &os, const vec2 &vec)
{
	return os << "x: " << vec.x << " y: " << vec.y;
}

inline std::ostream &operator<< (std::ostream &os, const vec3 &vec)
{
	return os << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z;
}

inline std::ostream &operator<< (std::ostream &os, const vec4 &vec)
{
	return os << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z << " w: " << vec.w;
}

#endif //ARCANE_GLM_H
