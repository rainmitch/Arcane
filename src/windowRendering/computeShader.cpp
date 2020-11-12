
#include <arcane/WindowRendering/computeShader.h>
#include <map>

namespace arcane
{
	unsigned int buildComputeShader (const std::string &name, const std::string &computeShader)
	{
		GLuint programID = glCreateProgram ();
		GLuint computeShaderID;
		GLint result;
		GLint infoLogLength;

		computeShaderID = glCreateShader (GL_COMPUTE_SHADER);
		const char *computeSrcPtr = computeShader.c_str ();
		glShaderSource (computeShaderID, 1, &computeSrcPtr, nullptr);
		glCompileShader (computeShaderID);
		// check the fragment shader
		glGetShaderiv (computeShaderID, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderiv (computeShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::string buffer = std::string (infoLogLength, ' ');
			glGetShaderInfoLog (computeShaderID, infoLogLength, nullptr, &buffer[0]);

			throw std::runtime_error ("Compute Shader<" + name + "> could not be built. Reason:\n" + buffer);
		}
		glAttachShader (programID, computeShaderID);
		glLinkProgram (programID);
		glGetProgramiv (programID, GL_LINK_STATUS, &result);
		if (!result)
		{
			glGetProgramiv (programID, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::string buffer = std::string (infoLogLength, ' ');
			glGetProgramInfoLog (programID, infoLogLength, nullptr, &buffer[0]);

			throw std::runtime_error ("Shader<" + name + "> could not be built. Reason:\n" + buffer);
		}

		glDetachShader (programID, computeShaderID);
		glDeleteShader (computeShaderID);

		return programID;
	}
	std::map<std::string, ComputeShader *> computeShaders;

	ComputeShader::ComputeShader (const std::string &name, const std::string &src) :
		ComputeShader (name, src,ComputeShaderInfo ()) {}

	ComputeShader::ComputeShader (const std::string &name, const std::string &src, const ComputeShaderInfo
		&computeShaderInfo)
	{
		m_computeShaderInfo = computeShaderInfo;

		m_programID = buildComputeShader (name, src);
		m_name = name;
	}

	void ComputeShader::use ()
	{
		glUseProgram (m_programID);
	}

	void ComputeShader::run (const unsigned int &x, const unsigned int &y, const unsigned int &z)
	{
		int counter = 0;
		for (auto &t : m_textures)
		{
			glActiveTexture (GL_TEXTURE0 + counter);
			glBindTexture (GL_TEXTURE_2D, std::get<1> (t));
			glUniform1i (glGetUniformLocation (m_programID, std::get<0> (t).c_str ()), counter);
			counter++;
		}
		glDispatchCompute (x, y, z);
	}

	void ComputeShader::createTexture (const std::string &name, const unsigned int &width, const unsigned int &height,
									   const ComputeShaderTextureFormat &textureFormat)
	{
		GLuint textureID;
		glGenTextures (1, &textureID);
		glActiveTexture(GL_TEXTURE0 + m_textures.size ());
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glBindImageTexture(m_textures.size (), textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		m_textures.emplace_back (std::make_tuple (name, textureID, textureFormat));
	}

	ArcBuffer ComputeShader::getImageResult ()
	{

	}

	unsigned int ComputeShader::getTextureID (const std::string &id)
	{
		for (auto &t : m_textures)
			if (std::get<0> (t) == id)
				return std::get<1> (t);
		throw std::runtime_error ("Computation shader<" + m_name + "> does not contain a texture with id<" +
			id + ">");
	}

	void ComputeShader::accessImage ()
	{
		glMemoryBarrier (GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void ComputeShader::createBufferObject (const std::string &name, const unsigned int &pos)
	{
		GLuint ssbo;
		glGenBuffers (1, &ssbo);

		m_bufferObjects[name] = {ssbo, pos};
	}

	void ComputeShader::uploadBufferObjectData (const std::string &name, void *data, const size_t &size,
											 const bool &staticUsage)
	{
		auto obj = m_bufferObjects[name];
		GLuint ssbo = std::get<0> ((obj));
		unsigned int pos = std::get<1> (obj);

		glBindBuffer (GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData (GL_SHADER_STORAGE_BUFFER, size, data, (staticUsage?GL_STATIC_DRAW:GL_DYNAMIC_DRAW));
		glBindBufferBase (GL_SHADER_STORAGE_BUFFER, pos, ssbo);
	}

	void registerComputeShader (const std::string &name, ComputeShader *computeShader)
	{
		computeShaders[name] = computeShader;
	}

	ComputeShader *getComputeShader (const std::string &name)
	{
		return computeShaders[name];
	}

	void deregisterComputeShader (const std::string &name)
	{
		computeShaders.erase (name);
	}
}