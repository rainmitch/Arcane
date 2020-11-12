
#include <arcane/WindowRendering/window.h>

#include <SDL2/SDL.h>

#include <arcane/scene.h>
#include <arcane/WindowRendering/renderer.h>
#include <map>


#include "../imgui.h"
#include "../imgui_impl_sdl.h"
#include "../imgui_impl_opengl3.h"

#ifdef WINDOWING_SYSTEM_SDL2

namespace arcane
{
	namespace Input
	{
		extern std::map<std::string, bool> keyDown;
		extern std::map<std::string, bool> keyPressed;
		extern std::string correctInput (const std::string &name);
	}

	namespace Window
	{
		extern Scene *currentScene;
		SDL_Window *win;
		bool windowRunning = true;

		#if defined (RENDERING_SYSTEM_OPENGL43)
			SDL_GLContext glContext;
		#endif

		void init (const std::string &title, const uint16_t &width, const uint16_t &height)
		{
			#if defined(RENDERING_SYSTEM_OPENGL43)
				SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 3);
				SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
				SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);

				SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, 1);
				SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, 4);
				SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			#endif

			if (SDL_Init (SDL_INIT_VIDEO) < 0)
			{
				throw std::runtime_error ("SDL could not be initialized! Reason:\n" + std::string (SDL_GetError()));
			}

			#if defined (RENDERING_SYSTEM_OPENGL43)

				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
				SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
				win = SDL_CreateWindow (title.c_str (), SDL_WINDOWPOS_CENTERED,
							SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
				if (win == nullptr)
				{
					throw std::runtime_error (SDL_GetError());
				}
				glContext = SDL_GL_CreateContext (win);
				if (glContext == nullptr)
				{
					throw std::runtime_error ("SDL could not create an OpenGL context! Reason:\n" +
					std::string (SDL_GetError ()));
				}

				SDL_GL_MakeCurrent (win, glContext);
			#endif
		}

		bool running ()
		{
			return windowRunning;
		}

		std::pair<unsigned int, unsigned int> size ()
		{
			unsigned int width, height;
			SDL_GetWindowSize (win, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));

			return {width, height};
		}

		void handleEvents ()
		{
			// clear pressed keys
			for (auto &pair : Input::keyPressed)
			{
				pair.second = false;
			}

			SDL_Event event;
			while (SDL_PollEvent (&event))
			{
				ImGui_ImplSDL2_ProcessEvent (&event);
				if (event.type == SDL_QUIT)
				{
					windowRunning = false;
				}
				else if (event.type == SDL_WINDOWEVENT)
				{
					if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						Renderer::resizeFrame (event.window.data1, event.window.data2);
					}
				}
				else if (event.type == SDL_KEYDOWN)
				{
					if (event.key.state == SDL_PRESSED)
					{
						std::string keyName = SDL_GetKeyName (event.key.keysym.sym);

						Input::keyDown[Input::correctInput (keyName)] = true;
						Input::keyPressed[Input::correctInput (keyName)] = true;
					}
				}
				else if (event.type == SDL_KEYUP)
				{
					if (event.key.state == SDL_RELEASED)
					{
						std::string keyName = SDL_GetKeyName (event.key.keysym.sym);

						Input::keyDown[Input::correctInput (keyName)] = false;
					}
				}
				else if (event.type == SDL_MOUSEBUTTONUP)
				{
					int x = event.button.x;
					int y = event.button.y;
				}
			}

			int x;
			int y;
			SDL_GetMouseState (&x, &y);
		}

		void render ()
		{/*

			currentScene->render();

			SDL_GL_SwapWindow (win);*/
		}
	}
}

#endif