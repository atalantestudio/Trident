// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticScene.hpp"
#include "AssetBuilder/API/Scene/StaticSurface.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	class StaticSceneBuilder {
		public:
			explicit StaticSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName);

			~StaticSceneBuilder();

			void addMaterialShader(sequence<char8> name, ShaderStage stage, sequence<char8> entryPoint, sequence<DescriptorSetLayoutBinding> bindings);

			void addMaterial(uint32 identifier, uint32 shaderIndex);

			void addSurface(const StaticSurface& surface);

			/// Compiles and returns an optimized static scene.
			StaticScene3D build() const;

		private:
			uint32 carveSurfaces(sequence<StaticSurface>& surfaces) const;

		private:
			std::filesystem::path rootOutputPath;
			view<char8> assetName;
			AssetLibrary* assetLibrary;
			std::vector<BuildMaterialShader> materialShaders;
			std::vector<BuildStaticMaterial3D> materials;
			std::vector<StaticSurface> surfaces;

			ref<GeometryAsset> surfaceGeometry;
	};
}