
#ifndef ARCANE_WINDOW_H
#define ARCANE_WINDOW_H

#include <iostream>

#include <arcane/scene.h>
#include <arcane/config.h>
#include <arcane/glm.h>


namespace arcane
{
	class Scene;
	namespace Window
	{
		void init (const std::string &title, const uint16_t &width, const uint16_t &height);

		bool running ();

		mat4 getProjectionMatrix ();
		std::pair<unsigned int, unsigned int> size ();

		void handleEvents ();

		void update ();
		void render ();

		void setScene (Scene *scene);
		Scene *getScene ();
	}
}

#endif //ARCANE_WINDOW_H
