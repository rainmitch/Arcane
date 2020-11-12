#ifndef ARCANE_OBJECT_H
#define ARCANE_OBJECT_H

#include <arcane/mesh.h>
#include <arcane/transform.h>
#include <arcane/threadSafe.h>
#include <arcane/script.h>
#include "scene.h"

namespace arcane
{
	class Object
	{
	public:
		Object (const bool &renderableObject=true);

		~Object ();

		void add (Script *script);

		bool renderableObject ();

		friend class Scene;

		Mesh mesh;
		Transform transform;
		vec3 scale = vec3 (1, 1, 1);

	private:
		std::vector<Script *> m_scripts;
		bool m_renderableObject;
	};
}


#endif //ARCANE_OBJECT_H
