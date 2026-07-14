// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticObject.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	class StaticSceneBuilder {
		public:
			explicit StaticSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName);

			~StaticSceneBuilder();

			StaticSceneBuilder& addMaterialShader(sequence<char8> name, ShaderStage stage, sequence<char8> entryPoint, sequence<DescriptorSetLayoutBinding> bindings);

			StaticSceneBuilder& addMaterial(uint32 identifier, uint32 shaderIndex);

			StaticSceneBuilder& createStaticObject(StaticObject object);

			/// Compiles and returns an optimized static scene.
			StaticScene3D build() const;

		private:
			std::filesystem::path rootOutputPath;
			view<char8> assetName;
			AssetLibrary* assetLibrary;
			std::vector<BuildMaterialShader> materialShaders;
			std::vector<BuildStaticMaterial3D> materials;
			std::vector<StaticObject> objects;
	};
}