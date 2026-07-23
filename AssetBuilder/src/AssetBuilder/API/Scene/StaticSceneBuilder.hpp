// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Lightmap.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"
#include "AssetBuilder/API/Scene/StaticSurface.hpp"

#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	class StaticSceneBuilder {
		private:
			static constexpr Extent2D LIGHTMAP_ATLAS_EXTENT = {1024, 1024};

		public:
			static StaticGeometry3D createWallGeometry(svec2<float32> scale);

		private:
			static LightmapNode* insertLightmapNode(Lightmap& lightmap, LightmapNode* rootNode);

		public:
			void addMaterialShader(sequence<char8> name, ShaderStage stage, sequence<char8> entryPoint, sequence<DescriptorSetLayoutBinding> bindings);

			uint32 addMaterial(ref<MaterialAsset> materialAsset);

			uint32 placeSurface(const SurfaceDescriptor& descriptor, const Transform& transform);

			uint32 snapSurface(const SurfaceDescriptor& descriptor, const StaticEdge& parentEdge, uint32 edgeIndex, float32 hingeAngle);
			uint32 snapSurface(const DualSnapSurfaceDescriptor& descriptor, const DualSnapEdge& edgeA, const DualSnapEdge& edgeB);

			/// Compiles and returns an optimized static scene.
			StaticScene3D build();

		private:
			uint32 compileSurface(const SurfaceDescriptor& descriptor, const Transform& transform, vec3<float32> hingeOffset, quat<float32> hingeRotation, bool textureFilteringTransform);

			void carveSurface(StaticSurface& surface, svec3<float32> origin, const std::vector<Carving>& carvings, uint32& surfaceVertexCount) const;

			void generateLightmaps(StaticScene3D& scene);

			void createLightmaps(StaticScene3D& scene);

			void updateLightmapCoordinates(StaticScene3D& scene);

		// TODO(asset-builder/scene): Keep exposing?
		public:
			std::vector<StaticSurface> surfaces;

		private:
			std::vector<svec3<float32>> vertices;
			std::vector<svec3<float32>> normals;
			std::vector<svec2<float32>> textureCoordinates;
			std::vector<std::vector<StaticTriangle3D>> trianglesPerMaterial;
			std::vector<BuildMaterialShader> materialShaders;
			std::vector<BuildStaticMaterial3D> materials;

			sequence<Lightmap> lightmaps;

			/// Sorted by ascending area.
			std::vector<LightmapLookup> lightmapLookups;

			uint32 triangleCount = 0;
			uint32 subsurfaceCount = 0;
			uint32 subsurfaceTextureCoordinateCount = 0;
	};
}