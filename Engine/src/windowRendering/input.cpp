
#include <arcane/WindowRendering/input.h>

#include <map>
#include <iostream>

#include "SDL2/input.h"


namespace arcane
{
	namespace Input
	{
		std::map<std::string, bool> keyDown;
		std::map<std::string, bool> keyPressed;
		std::map<std::string, std::string> mappedInputs;

		std::string correctInput (const std::string &input)
		{
			std::string out;

			for (char c : input)
			{
				if (c >= 'a' && c <= 'z')
				{
					out += 'A' + (c-'a');
				}
				else
				{
					out += c;
				}
			}

			return out;
		}

		void registerNewInput (const std::string &name, const std::string &input)
		{
			mappedInputs[name] = correctInput (input);
		}

		bool active (const std::string &name)
		{
			return keyDown[mappedInputs[name]];
		}

		bool pressed (const std::string &name)
		{
			return keyPressed[mappedInputs[name]];
		}
	}
}