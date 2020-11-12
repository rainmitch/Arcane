
#ifndef ARCANE_RENDERER_H
#define ARCANE_RENDERER_H

#include <string>

#include <arcane/threadSafe.h>
#include <arcane/glm.h>
#include <arcane/UI/arcBuffer.h>
#include <arcane/image.h>

namespace arcane
{
	class Mesh;


	namespace Renderer
	{
		extern void (*initialize) ();
		extern void (*registerMesh) (Mesh *mesh);
		extern void (*deregisterMesh) (Mesh *mesh);
		extern void (*renderFrame) (const mat4 &projectionMatrix, const mat4 &viewMatrix);
		extern void (*renderUI) (const ArcBuffer &buffer);
		extern void (*resizeFrame) (const unsigned int &width, const unsigned int &height);
		extern void (*setSkymap) (const std::vector<Image *> &images);

		extern int (*maxTextureSize) ();

		void configure ();
	}
}

#endif //ARCANE_RENDERER_H
