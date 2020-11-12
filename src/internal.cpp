
#include <arcane/internal.h>

#include <arcane/WindowRendering/window.h>
#include <arcane/jobSystem/JobSystem.h>
#include <arcane/WindowRendering/renderer.h>

#include <chrono>

extern std::function<void ()> rotateFunc;
namespace arcane
{
	void initializeArcane (const std::string &title, const uint16_t &width, const uint16_t &height)
	{
		Renderer::configure ();
		//JobSystem::init ();
		Window::init (title, width, height);
		Renderer::initialize ();
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

		while (Window::running ())
		{
			Window::handleEvents ();

			rotateFunc ();
			Window::update ();
			Window::render ();

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
	}
}