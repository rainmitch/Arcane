
#include "batchingObject.h"

#include <algorithm>
#include <arcane/WindowRendering/shader.h>

#include <arcane/WindowRendering/window.h>
#include <arcane/object.h>

namespace arcane
{
	namespace OpenGL43
	{
		BatchingObject::BatchingObject ()
		{
			initialize (Dynamic);
			m_changed = false;
			lastRender = new bool[0];
		}

		void BatchingObject::push (Mesh *mesh)
		{
			m_meshes.push_back (mesh);
			m_shaderName = mesh->getShader();
			m_changed = true;
			delete[] lastRender;
			lastRender = new bool[m_meshes.size ()];
		}

		void BatchingObject::remove (Mesh *mesh)
		{
			auto pos = std::find (m_meshes.begin (), m_meshes.end (), mesh);
			if (pos == m_meshes.end ())
				throw std::runtime_error ("Could not remove mesh from batch, mesh is not registered in batch!");
			m_meshes.erase (pos);
		}

		void BatchingObject::update ()
		{
			bool changed = false;
			for (Mesh *mesh : m_meshes)
			{
				if (mesh->changed ())
				{
					changed = true;
					break;
				}
			}


			if (changed)
			{
				std::vector<vec3> vertData;
				std::vector<vec3> normData;
				std::vector<unsigned int> indiceData;
				std::vector<vec2> uvData;
				size_t vertSize = 0;
				size_t normSize = 0;
				size_t indiceSize = 0;
				size_t uvSize = 0;

				if (!m_meshes.empty ())
				{
					auto albedo = m_meshes[0]->m_material.albedoMap;
					auto metalness = m_meshes[0]->m_material.metalnessMap;
					auto roughness = m_meshes[0]->m_material.roughnessMap;
					glBindTexture (GL_TEXTURE_2D, albedoID);
					glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, albedo->width, albedo->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
								  albedo->data.data ());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glBindTexture (GL_TEXTURE_2D, metalnessID);
					glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, metalness->width, metalness->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
								  metalness->data.data ());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					glBindTexture (GL_TEXTURE_2D, roughnessID);
					glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, roughness->width, roughness->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
								  roughness->data.data ());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}

				for (Mesh *mesh : m_meshes)
				{
					vertSize += mesh->m_vertices.size ();
					normSize += mesh->m_normals.size ();
					indiceSize += mesh->m_indices.size ();
					uvSize += mesh->m_texCoords.size ();
				}
				vertData = std::vector<vec3> ();
				vertData.reserve (vertSize);
				normData = std::vector<vec3> ();
				normData.reserve (normSize);
				indiceData = std::vector<unsigned int> ();
				indiceData.reserve (indiceSize);
				uvData = std::vector<vec2> ();
				uvData.reserve (uvSize);
				auto vertIt = vertData.begin ();
				auto normIt = normData.begin ();
				auto indiceIt = indiceData.begin ();
				auto uvIt = uvData.begin ();

				for (Mesh *mesh : m_meshes)
				{
					mesh->changed (true);
					auto verts = mesh->getVertices ();
					auto norms = mesh->getNormals ();
					auto indices = mesh->getIndices ();
					auto uvs = mesh->getTextureCoordinates ();
					vertData.insert (vertIt, verts.begin (), verts.end ());
					normData.insert (normIt, norms.begin (), norms.end ());
					indiceData.insert (indiceIt, indices.begin (), indices.end ());
					uvData.insert (uvIt, uvs.begin (), uvs.end ());

					vertIt += verts.size ();
					normIt += norms.size ();
					indiceIt += indices.size ();
					uvIt += uvs.size ();
				}


				glBindVertexArray (m_vao);
				glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
				glBufferData (GL_ARRAY_BUFFER, vertData.size () * sizeof (vec3), vertData.data (), GL_DYNAMIC_DRAW);

				glBindBuffer (GL_ARRAY_BUFFER, m_nbo);
				glBufferData (GL_ARRAY_BUFFER, normData.size () * sizeof (vec3), normData.data (), GL_DYNAMIC_DRAW);

				glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
				glBufferData (GL_ELEMENT_ARRAY_BUFFER, indiceData.size () * sizeof (unsigned int), indiceData.data (),
				  	GL_DYNAMIC_DRAW);

				glBindBuffer (GL_ARRAY_BUFFER, m_UVBO);
				glBufferData (GL_ARRAY_BUFFER, uvData.size () * sizeof (vec2), uvData.data (), GL_DYNAMIC_DRAW);
				m_changed = true;
			}
		}

		void BatchingObject::constructIndirectCommands ()
		{

		}

		void BatchingObject::render (const unsigned int &materialID, const mat4 &projectionMatrix, const mat4
		&viewMatrix)
		{
			glBindVertexArray (m_vao);

			GLuint programID = Shader::getShaderID (m_shaderName);

			glUseProgram (programID);

			glUniformMatrix4fv (glGetUniformLocation (programID, "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
			glUniformMatrix4fv (glGetUniformLocation (programID, "projectionMatrix"), 1, GL_FALSE,
								&projectionMatrix[0][0]);

			glBindBuffer (GL_ARRAY_BUFFER, m_modelMatricesBO);

			if (m_changed)
			{
				m_indirectCommands = std::vector<DrawElementsIndirectCommand> (m_meshes.size ());

				size_t indiceCounter = 0;
				size_t vertCounter = 0;
				for (size_t i = 0; i < m_meshes.size (); i++)
				{
					auto indices = m_meshes[i]->m_indices;
					auto verts = m_meshes[i]->m_vertices;

					m_indirectCommands[i].count = indices.size ();
					m_indirectCommands[i].instanceCount = 1;
					m_indirectCommands[i].firstIndex = indiceCounter;
					m_indirectCommands[i].baseVertex = vertCounter;
					m_indirectCommands[i].baseInstance = i;

					indiceCounter += indices.size ();
					vertCounter += verts.size ();
				}
				glBindBuffer (GL_DRAW_INDIRECT_BUFFER, m_ibo);
				glBufferData (GL_DRAW_INDIRECT_BUFFER, m_indirectCommands.size () *
													   sizeof (DrawElementsIndirectCommand), m_indirectCommands.data (), GL_DYNAMIC_DRAW);


				glBufferData (GL_ARRAY_BUFFER, m_meshes.size () * sizeof (mat4), nullptr, GL_DYNAMIC_DRAW);
				m_changed = false;
			}

			mat4 *modelMatrices = (mat4 *)glMapBuffer (GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				for (size_t i = 0; i < m_meshes.size (); i++)
				{
					mat4 modelMatrix = mat4 (1);
					Object *obj = m_meshes[i]->parent();
					modelMatrix = glm::translate (mat4 (1), obj->transform.position) *
							obj->transform.rotationMat4 () *
								  glm::scale (mat4 (1), obj->scale);
					modelMatrices[i] = modelMatrix;
				}
			glUnmapBuffer (GL_ARRAY_BUFFER);


			glUniform1ui (glGetUniformLocation (programID, "materialID"), materialID);

			glActiveTexture (GL_TEXTURE0);
			glBindTexture (GL_TEXTURE_2D, albedoID);
			glUniform1i (glGetUniformLocation (programID, "albedoMap"), 0);
			glActiveTexture (GL_TEXTURE1);
			glBindTexture (GL_TEXTURE_2D, metalnessID);
			glUniform1i (glGetUniformLocation (programID, "metalnessMap"), 1);
			glActiveTexture (GL_TEXTURE2);
			glBindTexture (GL_TEXTURE_2D, roughnessID);
			glUniform1i (glGetUniformLocation (programID, "roughnessMap"), 2);

			glMultiDrawElementsIndirect (GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, m_meshes.size (), 0);
		}

		std::string BatchingObject::shaderName ()
		{
			return m_shaderName;
		}
	}
}