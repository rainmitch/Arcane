#ifndef ARCANE_SCENE_H
#define ARCANE_SCENE_H

#include <arcane/object.h>
#include <arcane/camera.h>

#include <map>
#include <list>
#include <arcane/light.h>

namespace arcane
{
	class Scene
	{
	public:
		Scene ();

		void addObject (Object *object);
		void addLight (Light *light);

		void removeObject (Object *object);

		void setCamera (Camera *camera);
		Camera *getCamera ();

		void setSkybox (const std::vector<Image *> &images);

		void update ();

		const std::vector<Light *> &getLights ();

		void render ();
		
	private:
		void updateBoundBoxes ();
		bool *calculateCulling (const mat4 &projectionMatrix, const mat4 &viewMatrix);

		struct BoundingBoxData
		{
			vec3 pos;
			mat4 modelMat;
			float radius;
		};

		std::vector<BoundingBoxData> m_boundingBoxes;
		bool m_boundBoxesChanged;
		std::list<Object *> m_objects;
		std::vector<Light *> m_lights;
		Camera *m_currentCamera;
	};
}

#endif //ARCANE_SCENE_H
