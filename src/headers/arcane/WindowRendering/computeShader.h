
#ifndef ARCANE_COMPUTESHADER_H
#define ARCANE_COMPUTESHADER_H

#include <iostream>

#include <GL/glew.h>
#include <arcane/glm.h>
#include <vector>
#include <tuple>
#include <map>

namespace arcane
{
	unsigned int buildComputeShader (const std::string &name, const std::string &computeShader);

	struct ComputeShaderInfo
	{

	};

	typedef enum
	{
		R=GL_R,
		RG=GL_RG,
		RGB=GL_RGB,
		RGBA=GL_RGBA,
		BGRA=GL_BGRA,

		R8UI=GL_R8UI,
		R8I=GL_R8I,

		RG8UI=GL_RG8UI,
		RG8I=GL_RG8I,
		RG16UI=GL_RG16UI,
		RG16I=GL_RG16I,
		RG16F=GL_RG16F,
		RG32UI=GL_RG32UI,
		RG32I=GL_RG32I,
		RG32F=GL_RG32F,

		RGB8UI=GL_RGB8UI,
		RGB8I=GL_RGB8I,
		RGB16UI=GL_RGB16UI,
		RGB16I=GL_RGB16I,
		RGB16F=GL_RGB16F,
		RGB32UI=GL_RGB32UI,
		RGB32I=GL_RGB32I,
		RGB32F=GL_RGB32F,

		RGBA8UI=GL_RGBA8UI,
		RGBA8I=GL_RGBA8I,
		RGBA16UI=GL_RGBA16UI,
		RGBA16I=GL_RGBA16I,
		RGBA16F=GL_RGBA16F,
		RGBA32UI=GL_RGBA32UI,
		RGBA32I=GL_RGBA32I,
		RGBA32F=GL_RGBA32F,
	} ComputeShaderTextureFormat;

	class ComputeShader
	{
	public:
		ComputeShader (const std::string &name, const std::string &src);
		ComputeShader (const std::string &name, const std::string &src, const ComputeShaderInfo &computeShaderInfo);

		void use ();
		void run (const unsigned int &x, const unsigned int &y=1, const unsigned int &z=1);


		void createTexture (const std::string &name, const unsigned int &width, const unsigned
		int &height,
					  const ComputeShaderTextureFormat &textureFormat);

		unsigned int getTextureID (const std::string &id);

		template<typename T>
		void upload (const std::string &name, const T &val) {}
		template<typename T>
		void upload (const std::string &name, const size_t &size, const size_t &count, T *val) {}
		template<typename T>
		void upload (const std::string &name, const T &val, const bool &transpose) {}


		void createBufferObject (const std::string &name, const unsigned int &pos);
		void uploadBufferObjectData (const std::string &name, void *data, const size_t &size,
							   const bool &staticUsage=true);

		void accessImage ();

	private:
		ComputeShaderInfo m_computeShaderInfo;

		std::vector<std::tuple<std::string, GLuint, ComputeShaderTextureFormat>> m_textures;
		std::map<std::string, std::tuple<GLuint, unsigned int>> m_bufferObjects;

		std::string m_name;
		GLuint m_programID;
	};

	template<>
	inline void ComputeShader::upload (const std::string &name, const int &val)
	{
		glUniform1i (glGetUniformLocation (m_programID, name.c_str ()), val);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const float &val)
	{
		glUniform1f (glGetUniformLocation (m_programID, name.c_str ()), val);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const double &val)
	{
		glUniform1d (glGetUniformLocation (m_programID, name.c_str ()), val);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const unsigned int &val)
	{
		glUniform1ui (glGetUniformLocation (m_programID, name.c_str ()), val);
	}


	template<>
	inline void ComputeShader::upload (const std::string &name, const ivec2 &val)
	{
		glUniform2i (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const vec2 &val)
	{
		glUniform2f (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dvec2 &val)
	{
		glUniform2d (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const ivec3 &val)
	{
		glUniform3i (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const vec3 &val)
	{
		glUniform3f (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dvec3 &val)
	{
		glUniform3d (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const vec<3, unsigned int> &val)
	{
		glUniform3ui (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const ivec4 &val)
	{
		glUniform4i (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z, val.w);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const vec4 &val)
	{
		glUniform4f (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z, val.w);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dvec4 &val)
	{
		glUniform4d (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z, val.w);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const vec<4, unsigned int> &val)
	{
		glUniform4ui (glGetUniformLocation (m_programID, name.c_str ()), val.x, val.y, val.z, val.w);
	}


	template<>
	inline void ComputeShader::upload (const std::string &name, const size_t &size, const size_t &count, int *val)
	{
		if (size == 1)
			glUniform1iv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 2)
			glUniform2iv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 3)
			glUniform3iv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 4)
			glUniform4iv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const size_t &size, const size_t &count, float *val)
	{
		if (size == 1)
			glUniform1fv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 2)
			glUniform2fv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 3)
			glUniform3fv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 4)
			glUniform4fv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const size_t &size, const size_t &count, double *val)
	{
		if (size == 1)
			glUniform1dv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 2)
			glUniform2dv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 3)
			glUniform3dv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 4)
			glUniform4dv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const size_t &size, const size_t &count, unsigned int
	*val)
	{
		if (size == 1)
			glUniform1uiv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 2)
			glUniform2uiv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 3)
			glUniform3uiv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
		else if (size == 4)
			glUniform4uiv (glGetUniformLocation (m_programID, name.c_str ()), count, val);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const mat2 &val, const bool &transpose)
	{
		glUniformMatrix2fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const mat2x3 &val, const bool &transpose)
	{
		glUniformMatrix2x3fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const mat2x4 &val, const bool &transpose)
	{
		glUniformMatrix2x4fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}


	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat2 &val, const bool &transpose)
	{
		glUniformMatrix2dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat2x3 &val, const bool &transpose)
	{
		glUniformMatrix2x3dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat2x4 &val, const bool &transpose)
	{
		glUniformMatrix2x4dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}


	template<>
	inline void ComputeShader::upload (const std::string &name, const mat3 &val, const bool &transpose)
	{
		glUniformMatrix3fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const mat3x2 &val, const bool &transpose)
	{
		glUniformMatrix3x2fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const mat3x4 &val, const bool &transpose)
	{
		glUniformMatrix3x4fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat3 &val, const bool &transpose)
	{
		glUniformMatrix3dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat3x2 &val, const bool &transpose)
	{
		glUniformMatrix3x2dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat3x4 &val, const bool &transpose)
	{
		glUniformMatrix3x4dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}


	template<>
	inline void ComputeShader::upload (const std::string &name, const mat4 &val, const bool &transpose)
	{
		glUniformMatrix4fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const mat4x2 &val, const bool &transpose)
	{
		glUniformMatrix4x2fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const mat4x3 &val, const bool &transpose)
	{
		glUniformMatrix4x3fv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}


	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat4 &val, const bool &transpose)
	{
		glUniformMatrix4dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat4x2 &val, const bool &transpose)
	{
		glUniformMatrix4x2dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	template<>
	inline void ComputeShader::upload (const std::string &name, const dmat4x3 &val, const bool &transpose)
	{
		glUniformMatrix4x3dv (glGetUniformLocation (m_programID, name.c_str ()), 1, transpose, &val[0][0]);
	}

	void registerComputeShader (const std::string &name, ComputeShader *computeShader);
	ComputeShader *getComputeShader (const std::string &name);
	void deregisterComputeShader (const std::string &name);
}
#endif //ARCANE_COMPUTESHADER_H
