
#ifndef ARCANE_OPENGL43_RENDERER_H
#define ARCANE_OPENGL43_RENDERER_H

#include <arcane/threadSafe.h>
#include <arcane/glm.h>

#include <arcane/image.h>

namespace arcane
{
	class Mesh;
	namespace OpenGL43
	{
		void init ();
		void registerMesh (Mesh *mesh);
		void deregisterMesh (Mesh *mesh);

		void renderFrame (const mat4 &projectionMatrix, const mat4 &viewMatrix);
		//void renderUI (const ArcBuffer &buffer);
		void resizeFrame (const unsigned int &width, const unsigned int &height);
		void markRenderableObjects (bool *renderableObjects, const size_t &count);

		void setSkymap (const std::vector<Image *> &images);

		int maxTextureSize ();
	}
}

#endif //ARCANE_RENDERER_H
