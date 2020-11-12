#ifndef ARCANE_MATERIAL_H
#define ARCANE_MATERIAL_H

#include <arcane/glm.h>
#include <vector>
#include "image.h"

namespace arcane
{
	class Material
	{
	public:
		Material () = default;
		Material (const vec3 &albedo, const float &metallic, const float &roughness, const float &ao);

		void setAlbedoMap (Image *map);
		void setMetalnessMap (Image *map);
		void setRoughnessMap (Image *map);
		void setAoMap (Image *map);

		vec3 albedo;
		float metallic;
		float roughness;
		float ao;

		Image *albedoMap = nullptr;
		Image *roughnessMap = nullptr;
		Image *metalnessMap = nullptr;
		Image *aoMap = nullptr;
	};
}

#endif //ARCANE_MATERIAL_H
