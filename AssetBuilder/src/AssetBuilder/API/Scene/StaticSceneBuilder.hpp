// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticScene.hpp"
#include "AssetBuilder/API/Scene/StaticSurface.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	class StaticSceneBuilder {
		public:
			static StaticGeometry3D createWallGeometry(svec2<float32> scale);

		public:
			explicit StaticSceneBuilder(std::filesystem::path rootOutputPath);

			~StaticSceneBuilder();

			void addMaterialShader(sequence<char8> name, ShaderStage stage, sequence<char8> entryPoint, sequence<DescriptorSetLayoutBinding> bindings);

			void addMaterial(uint32 identifier, uint32 shaderIndex);

			uint32 placeSurface(const SurfaceDescriptor& descriptor, const Transform& transform);

			uint32 snapSurface(const SurfaceDescriptor& descriptor, uint32 parentSurfaceIndex, uint32 parentEdgeIndex, uint32 edgeIndex, float32 hingeAngle);

			/// Compiles and returns an optimized static scene.
			StaticScene3D build() const;

		private:
			uint32 compileSurface(const SurfaceDescriptor& descriptor, const Transform& transform, vec3<float32> hingeOffset, quat<float32> hingeRotation);

			void carveSurface(StaticSurface& surface, svec3<float32> origin, const std::vector<Carving>& carvings, uint32& surfaceVertexCount) const;

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