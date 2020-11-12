#ifndef ARCANE_SHADER_H
#define ARCANE_SHADER_H

#include <string>

namespace arcane
{
	namespace Shader
	{
		void registerShaders (const std::string &name, const std::string &vertexShader, const std::string
			&fragmentShader, const std::string &geometryShader="");
		void registerComputeShader (const std::string &name, const std::string &computeShader);

		uint32_t getShaderID (const std::string &name);
	}
}


#endif //ARCANE_SHADER_H
