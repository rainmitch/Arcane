
#ifndef ARCANE_INPUT_H
#define ARCANE_INPUT_H


#include <string>

namespace arcane
{
	namespace Input
	{
		void registerNewInput (const std::string &name, const std::string &input);
		bool active (const std::string &name);
		bool pressed (const std::string &name);
	}
}

#endif //ARCANE_INPUT_H
