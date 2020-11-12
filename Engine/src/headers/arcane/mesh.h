#ifndef ARCANE_MESH_H
#define ARCANE_MESH_H

#include <string>
#include <vector>
#include <cstdint>

#include <arcane/glm.h>
#include "material.h"

namespace arcane
{
	class MeshData
	{
	public:
		std::vector<vec3> vertices;
		std::vector<uint32_t> indices;
		std::vector<vec3> normals;
	};

	class Object;
	class Mesh
	{
	public:
		Mesh () = default;
		Mesh (const std::vector<vec3> &vertices, const std::vector<uint32_t> &indices, const std::vector<vec3>
		        &normals);
		Mesh (const std::vector<vec3> &vertices, const std::vector<uint32_t> &indices);

		void setVertices (const std::vector<vec3> &vertices);
		const std::vector<vec3> &getVertices ();

		void setIndices (const std::vector<uint32_t> &indices);
		const std::vector<uint32_t> &getIndices ();

		void setNormals (const std::vector<vec3> &normals);
		const std::vector<vec3> &getNormals ();

		void getMeshData (MeshData &meshData);
		void getMeshDataAndClearChanges (MeshData &meshData);
		bool changed (const bool &reset=false);

		Object *parent ();
		void parent (Object *parent);

		void setMaterial (const Material &material);
		Material *getMaterial ();

		void setTextureCoordinates (const std::vector<vec2> &texCoords);
		std::vector<vec2> getTextureCoordinates ();

		void setSmoothShading (const bool &value);

		void calculateSurfaceNormals ();
		void calculateVertexNormals ();

		std::string getShader ();

		std::vector<vec3> m_vertices;
		std::vector<uint32_t> m_indices;
		std::vector<vec3> m_normals;
		std::vector<vec2> m_texCoords;
		std::string m_shader = "default";
		Material m_material{};
		bool m_changed = false;
		bool m_smoothShading = true;
		Object *m_parent = nullptr;
	};
}

#endif //ARCANE_MESH_H
