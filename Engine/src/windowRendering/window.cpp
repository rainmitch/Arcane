
#include <arcane/WindowRendering/window.h>

#include <arcane/scene.h>

namespace arcane
{
	namespace Window
	{
		Scene *currentScene;

		void setScene (Scene *scene)
		{
			currentScene = scene;
		}

		Scene *getScene ()
		{
			return currentScene;
		}

		mat4 getProjectionMatrix ()
		{
			auto size = Window::size ();
			return glm::perspective (glm::radians (70.0f), (float)size.first/(float)size.second, 0.1f, 1000.0f);
		}

		void update ()
		{
			currentScene->update ();
		}
	}
}