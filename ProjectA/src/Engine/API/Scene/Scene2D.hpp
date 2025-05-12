#pragma once

#include "Engine/API/Object/Object2D.hpp"

namespace ProjectA {
	class Scene2D {
		public:
			uint32 getObjectCount() const;

			const Geometry2D& getGeometry(uint32 index) const;

			const ref<Material>& getMaterial(uint32 index) const;

			uint32 registerGeometry(const Geometry2DDescriptor& descriptor);

			template<typename MaterialType, typename... Argument>
			uint32 registerMaterial(Argument&&... arguments) {
				const uint32 materialIndex = static_cast<uint32>(materials.size());

				materials.emplace_back(createRef<MaterialType>(std::forward<Argument>(arguments)...));

				return materialIndex;
			}

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