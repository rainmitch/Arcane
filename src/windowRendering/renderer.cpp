//
// Created by Rain on 9/8/2020.
//

#include <arcane/WindowRendering/renderer.h>

#include <arcane/mesh.h>

#include "OpenGL43/renderer.h"

namespace arcane
{
	namespace Renderer
	{
		void (*initialize) ();
		void (*registerMesh) (Mesh *mesh);
		void (*deregisterMesh) (Mesh *mesh);
		void (*renderFrame) (const mat4 &projectionMatrix, const mat4 &viewMatrix);
		void (*renderUI) (const ArcBuffer &buffer);
		void (*resizeFrame) (const unsigned int &width, const unsigned int &height);
		void (*setSkymap) (const std::vector<Image *> &images);

		int (*maxTextureSize) ();

		void configure ()
		{
			// todo implement runtime detection

			initialize = OpenGL43::init;
			registerMesh = OpenGL43::registerMesh;
			deregisterMesh = OpenGL43::deregisterMesh;
			renderFrame = OpenGL43::renderFrame;
			renderUI = OpenGL43::renderUI;
			resizeFrame = OpenGL43::resizeFrame;
			setSkymap = OpenGL43::setSkymap;

			maxTextureSize = OpenGL43::maxTextureSize;
		}
	}
}