
#include <arcane/mesh.h>
#include <string>
#include <iostream>

namespace arcane
{
	void Mesh::setVertices (const std::vector<vec3> &vertices)
	{
		m_vertices = vertices;
		m_changed = true;
	}
	Mesh::Mesh (const std::vector<vec3> &vertices, const std::vector<uint32_t> &indices,
				const std::vector<vec3> &normals)
	{
		m_vertices = vertices;
		m_indices = indices;
		m_normals = normals;
		m_changed = true;
	}

	Mesh::Mesh (const std::vector<vec3> &vertices, const std::vector<uint32_t> &indices)
	{
		m_vertices = vertices;
		m_indices = indices;
		calculateVertexNormals ();
		m_changed = true;
	}

	const std::vector<vec3> &Mesh::getVertices ()
	{
		return m_vertices;
	}


	void Mesh::setIndices (const std::vector<uint32_t> &indices)
	{
		m_indices = indices;
		m_changed = true;
	}

	const std::vector<uint32_t> &Mesh::getIndices ()
	{
		return m_indices;
	}


	void Mesh::setNormals (const std::vector<vec3> &normals)
	{
		m_normals = normals;
		m_changed = true;
	}

	const std::vector<vec3> &Mesh::getNormals ()
	{
		return m_normals;
	}


	void Mesh::getMeshData (MeshData &meshData)
	{
		meshData.vertices.insert (meshData.vertices.end (), m_vertices.begin (), m_vertices.end ());
		meshData.indices.insert (meshData.indices.end (), m_indices.begin (), m_indices.end ());
		meshData.normals.insert (meshData.normals.end (), m_normals.begin (), m_normals.end ());
	}

	void Mesh::getMeshDataAndClearChanges (MeshData &meshData)
	{
		if (m_changed)
		{
			meshData.vertices.insert (meshData.vertices.end (), m_vertices.begin (), m_vertices.end ());
			meshData.indices.insert (meshData.indices.end (), m_indices.begin (), m_indices.end ());
			meshData.normals.insert (meshData.normals.end (), m_normals.begin (), m_normals.end ());
			m_changed = false;
		}
	}

	bool Mesh::changed (const bool &reset)
	{
		bool b = m_changed;
		if (reset)
			m_changed = false;
		return b;
	}

	void Mesh::setMaterial (const Material &material)
	{
		m_material = material;
	}

	Material *Mesh::getMaterial ()
	{
		return &m_material;
	}

	void Mesh::setTextureCoordinates (const std::vector<vec2> &texCoords)
	{
		m_texCoords = texCoords;
		m_changed = true;
	}

	std::vector<vec2> Mesh::getTextureCoordinates ()
	{
		return m_texCoords;
	}

	void Mesh::setSmoothShading (const bool &value)
	{
		m_smoothShading = value;
	}

	void Mesh::calculateSurfaceNormals ()
	{
		m_normals = std::vector<vec3> (m_vertices.size ());
		for (size_t i = 0; i < m_indices.size (); i+=3)
		{
			vec3 U = m_vertices[m_indices[i+1]] - m_vertices[m_indices[i+0]];
			vec3 V = m_vertices[m_indices[i+2]] - m_vertices[m_indices[i+0]];
			vec3 N = normalize (cross (U, V));

			m_normals[m_indices[i+0]] = N;
			m_normals[m_indices[i+1]] = N;
			m_normals[m_indices[i+2]] = N;
		}

		m_changed = true;
	}

	void Mesh::calculateVertexNormals ()
	{
		m_normals = std::vector<vec3> (m_vertices.size (), vec3 (0, 0, 0));

		for (size_t i = 0; i < m_indices.size (); i+=3)
		{
			uint32_t a = m_indices[i+0];
			uint32_t b = m_indices[i+1];
			uint32_t c = m_indices[i+2];

			vec3 U = m_vertices[b] - m_vertices[a];
			vec3 V = m_vertices[c] - m_vertices[a];
			vec3 N = cross (U, V);

			m_normals[a] += N;
			m_normals[b] += N;
			m_normals[c] += N;
		}

		for (size_t i = 0; i < m_normals.size (); i++)
		{
			m_normals[i] = normalize (m_normals[i]);
		}

		m_changed = true;
	}

	std::string Mesh::getShader ()
	{
		return m_shader;
	}

	Object *Mesh::parent ()
	{
		return m_parent;
	}

	void Mesh::parent (Object *parent)
	{
		m_parent = parent;
	}
}