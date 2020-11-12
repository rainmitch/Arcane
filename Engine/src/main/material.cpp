
#include <arcane/material.h>

namespace arcane
{
	Material::Material (const vec3 &albedo, const float &metallic, const float &roughness, const float &ao)
	{
		this->albedo = albedo;
		this->metallic = metallic;
		this->roughness = roughness;
		this->ao = ao;
	}

	void Material::setAlbedoMap (Image *map)
	{
		albedoMap = map;
	}

	void Material::setMetalnessMap (Image *map)
	{
		metalnessMap = map;
	}

	void Material::setRoughnessMap (Image *map)
	{
		roughnessMap = map;
	}

	void Material::setAoMap (Image *map)
	{
		aoMap = map;
	}
}