// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticMaterial.hpp"
#include "AssetBuilder/API/Scene/StaticObject.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	class StaticSceneBuilder {
		public:
			explicit StaticSceneBuilder(std::filesystem::path rootOutputPath);

			~StaticSceneBuilder();

			StaticSceneBuilder& allocateMaterials(uint32 count);

			StaticSceneBuilder& insertMaterial(uint32 index, const StaticMaterial& material);

			StaticSceneBuilder& createStaticObject(const StaticObjectDescriptor& descriptor);

			/// Compiles and returns an optimized static scene.
			StaticScene3D build() const;

		private:
			std::filesystem::path rootOutputPath;
			AssetLibrary* assetLibrary;

			sequence<StaticMaterial> materials;
			std::vector<StaticObject> objects;
	};
}