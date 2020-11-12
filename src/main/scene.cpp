
#include <arcane/scene.h>
#include <map>

#include <arcane/WindowRendering/renderer.h>
#include <arcane/WindowRendering/window.h>

#include <algorithm>
#include <arcane/WindowRendering/computeShader.h>

#include "../windowRendering/OpenGL43/renderer.h"

namespace arcane
{
	Scene::Scene ()
	{

	}

	void Scene::addObject (Object *object)
	{
		auto pos = std::find (m_objects.begin (), m_objects.end (), object);
		if (pos != m_objects.end ())
		{
			throw std::runtime_error ("Scene already contains object!");
		}

		if (object->renderableObject ())
			Renderer::registerMesh (&object->mesh);

		for (Script *script : object->m_scripts)
		{
			script->Start ();
		}
		m_objects.push_back (object);
		m_boundingBoxes.emplace_back (BoundingBoxData ());
	}

	void Scene::addLight (Light *light)
	{
		m_lights.push_back (light);
	}


	void Scene::removeObject (Object *object)
	{
		auto pos = std::find (m_objects.begin (), m_objects.end (), object);
		if (pos == m_objects.end ())
		{
			throw std::runtime_error ("Scene does not contain object!");
		}

		Renderer::deregisterMesh (&object->mesh);
		m_objects.erase (pos);
	}


	void Scene::setCamera (Camera *camera)
	{
		m_currentCamera = camera;
	}

	Camera *Scene::getCamera ()
	{
		return m_currentCamera;
	}

	void Scene::update ()
	{
		for (Object *obj : m_objects)
		{
			for (Script *script : obj->m_scripts)
			{
				script->Update ();
			}
		}
	}

	const std::vector<Light *> &Scene::getLights ()
	{
		return m_lights;
	}

	ComputeShader *boundingBoxComputeShader = nullptr;

	void Scene::render ()
	{
		if (boundingBoxComputeShader == nullptr)
			boundingBoxComputeShader = getComputeShader ("FrustumCulling");

		mat4 projectionMatrix = Window::getProjectionMatrix ();
		mat4 viewMatrix = Window::getScene ()->getCamera ()->getViewMatrix ();

		//updateBoundBoxes ();
		/*boundingBoxComputeShader->use ();
		boundingBoxComputeShader->uploadBufferObjectData ("boundingBoxes", &m_boundingBoxes[0],
												 m_boundingBoxes.size() * sizeof (BoundingBoxData), false);

		boundingBoxComputeShader->upload ("numberOfBoxes", (int)m_boundingBoxes.size ());

		boundingBoxComputeShader->run (m_boundingBoxes.size()/16);*/

		//bool *renderableObjs = calculateCulling (projectionMatrix, viewMatrix);

		//OpenGL43::markRenderableObjects (renderableObjs, m_objects.size ());
		Renderer::renderFrame (projectionMatrix, viewMatrix);

		//delete[] renderableObjs;
	}

	void Scene::updateBoundBoxes ()
	{
		size_t counter = 0;
		for (Object *obj : m_objects)
		{
			if (!obj->mesh.changed ())
			{
				continue;
			}
			vec3 low;
			vec3 high;
			auto verts = obj->mesh.m_vertices;
			if (verts.empty ())
				continue;
			low = verts[0];
			high = verts[0];
			for (size_t i = 1; i < verts.size (); i++)
			{
				if (verts[i].x < low.x)
					low.x = verts[i].x;
				if (verts[i].y < low.y)
					low.y = verts[i].y;
				if (verts[i].z < low.z)
					low.z = verts[i].z;

				if (verts[i].x > high.x)
					high.x = verts[i].x;
				if (verts[i].y > high.y)
					high.y = verts[i].y;
				if (verts[i].z > high.z)
					high.z = verts[i].z;
			}
			mat4 m (1);
			m = glm::translate (m, obj->transform.position);
			m *= obj->transform.rotationMat4 ();
			m *= glm::scale (obj->scale);
			mat3 m2 = mat3 (m);

			low = m2*low;
			high = m2*high;

			float lowR = distance (obj->transform.position, low) / 2;
			float highR = distance (obj->transform.position, high) / 2;

			float r = max (lowR, highR);

			m_boundingBoxes[counter] = {obj->transform.position, m, r};
			counter++;
		}
	}

	bool *Scene::calculateCulling (const mat4 &projectionMatrix, const mat4 &viewMatrix)
	{
		bool *objs = new bool[m_objects.size ()];
		size_t counter = 0;

		for (const BoundingBoxData &boundingBoxData : m_boundingBoxes)
		{
			//vec3 frustumPos = boundingBoxData.pos * boundingBoxData.modelMat * viewMat * projMat;
			vec4 frustumPos = (projectionMatrix * viewMatrix * boundingBoxData.modelMat) * vec4 (boundingBoxData.pos,1);

			frustumPos = frustumPos / frustumPos.w;
			frustumPos.z = frustumPos.z * 0.5f + 0.5f;
			//std::cout << frustumPos << std::endl;

			if (frustumPos.x >= -1 && frustumPos.x <= 1 && frustumPos.y >= -1 && frustumPos.y <= 1 &&
				frustumPos.z >= 0 && frustumPos.z <= 1)
				objs[counter] = true;
			else
				objs[counter] = false;
			counter++;
		}

		return objs;
	}

	void Scene::setSkybox (const std::vector<Image *> &images)
	{
		Renderer::setSkymap (images);
	}
}