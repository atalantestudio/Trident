#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Object/Object2D.hpp"

namespace ProjectA {
	class Scene2D {
		public:
			uint32 registerGeometry(const Geometry2D& geometry);

			uint32 registerMaterial(const ref<Material>& material);

			uint32 registerObject(const Object2D& object);

		private:
			chain<Geometry2D> geometries;
			chain<ref<Material>> materials;
			chain<Object2D> objects;
	};
}