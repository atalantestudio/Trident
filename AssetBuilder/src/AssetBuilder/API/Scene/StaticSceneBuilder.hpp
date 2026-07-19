// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticScene.hpp"
#include "AssetBuilder/API/Scene/StaticSurface.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	class StaticSceneBuilder {
		public:
			explicit StaticSceneBuilder(std::filesystem::path rootOutputPath);

			~StaticSceneBuilder();

			void addMaterialShader(sequence<char8> name, ShaderStage stage, sequence<char8> entryPoint, sequence<DescriptorSetLayoutBinding> bindings);

			void addMaterial(uint32 identifier, uint32 shaderIndex);

			void placeSurface(const SurfaceDescriptor& descriptor, const Transform& transform);

			// TODO(asset-builder/scene/surface-snapping)
			// void snapSurface(const StaticGeometry3D& geometry, const SurfaceDescriptor& descriptor, uint32 existingEdgeIndex, uint32 newEdgeIndex);

			/// Compiles and returns an optimized static scene.
			StaticScene3D build() const;

		private:
			// TODO(asset-builder/scene): Restore.
			// void carveSurface(StaticSurface& surface, const std::vector<Carving>& carvings) const;

		private:
			std::filesystem::path rootOutputPath;
			AssetLibrary* assetLibrary;
			std::vector<BuildMaterialShader> materialShaders;
			std::vector<BuildStaticMaterial3D> materials;
			std::vector<StaticSurface> surfaces;

			std::vector<svec3<float32>> vertices;
			std::vector<svec3<float32>> normals;
			std::vector<svec2<float32>> textureCoordinates;
			std::vector<std::vector<StaticTriangle3D>> trianglesPerMaterial;

			uint32 subsurfaceCount = 0;
			uint32 triangleCount = 0;
	};
}