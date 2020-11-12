
#ifndef ARCANE_BATCHINGOBJECT_H
#define ARCANE_BATCHINGOBJECT_H


#include <vector>
#include <arcane/mesh.h>
#include <GL/glew.h>

#include <arcane/WindowRendering/shader.h>

namespace arcane
{
	namespace OpenGL43
	{
		enum BufferDrawType
		{
			Static,
			Dynamic
		};

		class BatchingObject
		{
		public:
			BatchingObject ();

			void push (Mesh *mesh);
			void remove (Mesh *mesh);

			void update ();

			void constructIndirectCommands ();
			void render (const unsigned int &materialID, const mat4 &projectionMatrix, const mat4 &viewMatrix);

			std::string shaderName ();


			void initialize (const BufferDrawType &drawType)
			{
				glGenVertexArrays (1, &m_vao);
				glBindVertexArray (m_vao);
				glGenBuffers (1, &m_vbo);
				glGenBuffers (1, &m_nbo);
				glGenBuffers (1, &m_ebo);
				glGenBuffers (1, &m_ibo);
				glGenBuffers (1, &m_UVBO);
				glGenBuffers (1, &m_modelMatricesBO);

				glGenTextures (1, &albedoID);
				glGenTextures (1, &metalnessID);
				glGenTextures (1, &roughnessID);

				glUseProgram (Shader::getShaderID (shaderName ()));

				glEnableVertexAttribArray (0);
				glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
				glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

				glEnableVertexAttribArray (1);
				glBindBuffer (GL_ARRAY_BUFFER, m_nbo);
				glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

				glEnableVertexAttribArray (2);
				glVertexAttribIPointer (2, 1, GL_UNSIGNED_INT, sizeof (DrawElementsIndirectCommand), (void*)(
						offsetof (DrawElementsIndirectCommand, baseInstance)));
				glVertexAttribDivisor (2, 1);

				glEnableVertexAttribArray (3);
				glBindBuffer (GL_ARRAY_BUFFER, m_UVBO);
				glVertexAttribPointer (3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

				glBindBuffer (GL_ARRAY_BUFFER, m_modelMatricesBO);
				const unsigned int modelPos = 4;
				for (size_t i = 0; i < 4; i++)
				{
					glEnableVertexAttribArray (modelPos + i);
					glVertexAttribPointer (4+i, 4, GL_FLOAT, GL_FALSE, sizeof (mat4), (void *) (sizeof (vec4) * i));
					glVertexAttribDivisor (modelPos+i, 1);
				}

				this->m_drawType = drawType;
			}

		private:
			typedef  struct {
				uint  count;
				uint  instanceCount;
				uint  firstIndex;
				uint  baseVertex;
				uint  baseInstance;
			} DrawElementsIndirectCommand;

			GLuint m_vao;
			GLuint m_vbo;
			GLuint m_nbo;
			GLuint m_ebo;
			GLuint m_ibo;
			GLuint m_UVBO;
			GLuint m_modelMatricesBO;
			std::vector<Mesh *> m_meshes;
			std::vector<DrawElementsIndirectCommand> m_indirectCommands;
			BufferDrawType m_drawType;
			std::string m_shaderName;
			bool m_changed;
			bool *lastRender;

			GLuint albedoID;
			GLuint metalnessID;
			GLuint roughnessID;
		};
	}
}
#endif //ARCANE_BATCHINGOBJECT_H
