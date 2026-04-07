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

			inline const sequence<Geometry2D>& getGeometries() const {
				return geometries;
			}

			inline const sequence<Hull2D>& getHulls() const {
				return hulls;
			}

		private:
			void loadGeometries(ref<Scene2DAsset> sceneAsset);
			void loadEntityComponentMappings(ref<Scene2DAsset> sceneAsset);

		private:
			sequence<Geometry2D> geometries;
			sequence<Hull2D> hulls;
	};
}