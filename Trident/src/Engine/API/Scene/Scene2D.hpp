// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Scene/Scene.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"

namespace Trident {
	class Scene2D : public Scene {
		public:
			explicit Scene2D(ref<Scene2DAsset> sceneAsset);

			inline uint32 getStaticVertexCount() const {
				return staticVertexCount;
			}

			inline uint32 getStaticIndexCount() const {
				return staticIndexCount;
			}

			inline uint32 getDynamicVertexCount() const {
				return dynamicVertexCount;
			}

			inline uint32 getDynamicIndexCount() const {
				return dynamicIndexCount;
			}

			inline const sequence<uint8>& getDataBuffer() const {
				return dataBuffer;
			}

			inline const sequence<Geometry2D>& getGeometries() const {
				return geometries;
			}

			inline const sequence<Hull2D>& getHulls() const {
				return hulls;
			}

		private:
			void loadGeometries(ref<Scene2DAsset> sceneAsset);
			void loadHulls(ref<Scene2DAsset> sceneAsset);
			void loadEntityComponentMappings(ref<Scene2DAsset> sceneAsset);

		private:
			uint32 staticVertexCount;
			uint32 staticIndexCount;

			/// Contains static (internal to the scene) and dynamic (external geometries) vertices and indices.
			sequence<uint8> dataBuffer;

			uint32 dynamicVertexCount = 0;
			uint32 dynamicIndexCount = 0;
			sequence<Geometry2D> geometries;
			sequence<Hull2D> hulls;
	};
}