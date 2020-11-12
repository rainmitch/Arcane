
#include <arcane/WindowRendering/shader.h>
#include <arcane/config.h>

#include <arcane/WindowRendering/renderer.h>

#include <map>

#if defined(RENDERING_SYSTEM_OPENGL)
#include <GL/glew.h>

namespace arcane
{
	namespace Shader
	{
		std::map<std::string, uint32_t> shaders;


		void registerShaders (const std::string &name, const std::string &vertexShader, const std::string
			&fragmentShader, const std::string &geometryShader)
		{
			// Create the shaders
			GLuint vertexShaderID = glCreateShader (GL_VERTEX_SHADER);
			GLuint fragmentShaderID = glCreateShader (GL_FRAGMENT_SHADER);
			GLuint geometryShaderID;

			// Compile the shaders
			GLint result = GL_FALSE;
			int infoLogLength;

			const char *vertexSrcPtr = vertexShader.c_str ();
			glShaderSource (vertexShaderID, 1, &vertexSrcPtr, nullptr);
			glCompileShader (vertexShaderID);

			// check the vertex shader
			glGetShaderiv (vertexShaderID, GL_COMPILE_STATUS, &result);
			if (!result)
			{
				glGetShaderiv (vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::string buffer = std::string (infoLogLength, ' ');
				glGetShaderInfoLog (vertexShaderID, infoLogLength, nullptr, &buffer[0]);

				throw std::runtime_error ("Vertex Shader<" + name + "> could not be built. Reason:\n" + buffer);
			}

			const char *fragmentSrcPtr = fragmentShader.c_str ();
			glShaderSource (fragmentShaderID, 1, &fragmentSrcPtr, nullptr);
			glCompileShader (fragmentShaderID);

			// check the fragment shader
			glGetShaderiv (fragmentShaderID, GL_COMPILE_STATUS, &result);
			if (!result)
			{
				glGetShaderiv (fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::string buffer = std::string (infoLogLength, ' ');
				glGetShaderInfoLog (fragmentShaderID, infoLogLength, nullptr, &buffer[0]);

				throw std::runtime_error ("Fragment Shader<" + name + "> could not be built. Reason:\n" + buffer);
			}

			if (!geometryShader.empty ())
			{
				geometryShaderID = glCreateShader (GL_GEOMETRY_SHADER);
				const char *geometrySrcPtr = geometryShader.c_str ();
				glShaderSource (geometryShaderID, 1, &geometrySrcPtr, nullptr);
				glCompileShader (geometryShaderID);

				// check the fragment shader
				glGetShaderiv (geometryShaderID, GL_COMPILE_STATUS, &result);
				if (!result)
				{
					glGetShaderiv (geometryShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
					std::string buffer = std::string (infoLogLength, ' ');
					glGetShaderInfoLog (geometryShaderID, infoLogLength, nullptr, &buffer[0]);

					throw std::runtime_error ("Geometry Shader<" + name + "> could not be built. Reason:\n" + buffer);
				}
			}

			// link the program
			GLuint programID = glCreateProgram ();
			glAttachShader (programID, vertexShaderID);
			glAttachShader (programID, fragmentShaderID);
			if (!geometryShader.empty ())
				glAttachShader (programID, geometryShaderID);

			glLinkProgram (programID);

			// check the program
			glGetProgramiv (programID, GL_LINK_STATUS, &result);
			if (!result)
			{
				glGetProgramiv (programID, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::string buffer = std::string (infoLogLength, ' ');
				glGetProgramInfoLog (programID, infoLogLength, nullptr, &buffer[0]);

				throw std::runtime_error ("Shader<" + name + "> could not be built. Reason:\n" + buffer);
			}

			glDetachShader (programID, vertexShaderID);
			glDetachShader (programID, fragmentShaderID);
			if (!geometryShader.empty ())
				glDetachShader (programID, geometryShaderID);

			glDeleteShader (vertexShaderID);
			glDeleteShader (fragmentShaderID);
			if (!geometryShader.empty ())
				glDeleteShader (geometryShaderID);

			shaders[name] = programID;
		}

		uint32_t getShaderID (const std::string &name)
		{
			return shaders[name];
		}
	}
}

#endif
