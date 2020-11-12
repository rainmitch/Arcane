
#include <arcane/internal.h>

#include <arcane/WindowRendering/window.h>
#include <arcane/jobSystem/JobSystem.h>
#include <arcane/WindowRendering/renderer.h>

#include <chrono>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <SDL.h>

#include <GL/glew.h>

extern void MainGUI ();
extern std::function<void ()> rotateFunc;
namespace arcane
{
	namespace Window
	{
		extern SDL_Window *win;
		extern SDL_GLContext glContext;
	}
	void initializeImGui ()
	{
	}

	void initializeArcane (const std::string &title, const uint16_t &width, const uint16_t &height)
	{
		Renderer::configure ();
		//JobSystem::init ();
		Window::init (title, width, height);
		Renderer::initialize ();

		//initializeImGui ();
	}

	namespace Time
	{
		extern float internalDelta;
	}

	void startArcane ()
	{
		auto start = std::chrono::high_resolution_clock::now ();
		auto lastTime = std::chrono::high_resolution_clock::now ();
		uint fps = 0;

		IMGUI_CHECKVERSION ();
		ImGui::CreateContext ();

		ImGuiIO& io = ImGui::GetIO (); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		ImGui::StyleColorsDark ();
		ImGui_ImplSDL2_InitForOpenGL (Window::win, Window::glContext);
		ImGui_ImplOpenGL3_Init ("#version 150");
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		while (Window::running ())
		{
			Window::handleEvents ();

			rotateFunc ();
			Window::update ();

			//Window::render ();
			Window::getScene ()->render();

			ImGui_ImplOpenGL3_NewFrame ();
			ImGui_ImplSDL2_NewFrame (Window::win);
			ImGui::NewFrame ();

			MainGUI ();
			Window::getScene ()->runGUI ();

			ImGui::Render();
			auto s = Window::size ();
			//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
			//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			//glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			SDL_GL_SwapWindow (Window::win);

			if (std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now () - start)
			        .count () >= 1000)
			{
				start = std::chrono::high_resolution_clock::now ();
				std::cout << fps << " fps" << std::endl;
				fps = 0;
			}
			else
			{
				fps++;
			}
			Time::internalDelta = std::chrono::duration_cast<std::chrono::milliseconds> (
					std::chrono::high_resolution_clock::now () - lastTime).count ()/1000.0f;
			lastTime = std::chrono::high_resolution_clock::now ();
		}

		// kill job system
		//JobSystem::terminate ();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}