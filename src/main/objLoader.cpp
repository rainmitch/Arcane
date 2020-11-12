
#include <arcane/objLoader.h>

#include <limits>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobj.h"

namespace arcane
{
	Mesh loadOBJ (const std::string &path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj (&attrib, &shapes, &materials, &warn, &err, path.c_str ());

		if (!warn.empty())
		{
			throw std::runtime_error (warn);
		}

		if (!err.empty())
		{
			throw std::runtime_error (err);
		}

		auto shape = shapes[0];
		auto mesh = shape.mesh;

		std::vector<vec3> verts = std::vector<vec3> (attrib.vertices.size ()/3);
		std::vector<uint32_t> indices = std::vector<uint32_t> (mesh.indices.size ());
		std::vector<vec2> texCoords = std::vector<vec2> (attrib.texcoords.size () / 2);

		for (size_t i = 0; i < attrib.vertices.size ()/3; i++)
		{
			verts[i].x = attrib.vertices[i*3+0];
			verts[i].y = attrib.vertices[i*3+1];
			verts[i].z = attrib.vertices[i*3+2];
		}

		for (size_t i = 0; i < mesh.indices.size (); i++)
		{
			indices[i] = mesh.indices[i].vertex_index;
		}

		for (size_t i = 0; i < attrib.texcoords.size ()/2; i++)
		{
			texCoords[i].x = attrib.texcoords[i*2+0];
			texCoords[i].y = attrib.texcoords[i*2+1];
		}

		Mesh m = Mesh (verts, indices);
		m.setTextureCoordinates (texCoords);

		return m;
	}
}
