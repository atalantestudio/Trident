#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Object/Object2D.hpp"

namespace ProjectA {
	class Scene2D {
		public:
			uint32 getObjectCount() const;

			const Geometry2D& getGeometry(uint32 index) const;

			const ref<Material>& getMaterial(uint32 index) const;

			uint32 registerGeometry(const Geometry2D& geometry);

			uint32 registerMaterial(const ref<Material>& material);

			// Creates and registers an object into the scene.
			Object2D& registerObject(const Object2DDescriptor& descriptor);

			void updateTransform(Object2D& object) const;

			std::vector<Object2D>::iterator begin();
			std::vector<Object2D>::const_iterator begin() const;

			std::vector<Object2D>::iterator end();
			std::vector<Object2D>::const_iterator end() const;

			const Object2D& operator [](std::size_t index) const;

			Object2D& operator [](std::size_t index);

		private:
			chain<Geometry2D> geometries;
			chain<ref<Material>> materials;
			chain<Object2D> objects;
	};
}