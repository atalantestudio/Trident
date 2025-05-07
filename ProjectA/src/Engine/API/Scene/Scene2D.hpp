#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Object/Object2D.hpp"

namespace ProjectA {
	class Scene2D {
		public:
			const Geometry2D& getGeometry(uint32 index) const;

			const ref<Material>& getMaterial(uint32 index) const;

			uint32 registerGeometry(const Geometry2D& geometry);

			uint32 registerMaterial(const ref<Material>& material);

			uint32 registerObject(const Object2D& object);

			std::vector<Object2D>::const_iterator begin() const;

			std::vector<Object2D>::const_iterator end() const;

		private:
			chain<Geometry2D> geometries;
			chain<ref<Material>> materials;
			chain<Object2D> objects;
	};
}