// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "AssetBuilder/API/Scene/Lightmap.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"

namespace Trident {
	struct LightmapNode;

	class Lightmapper {
		public:
			static constexpr Extent2D ATLAS_SIZE = {2048, 2048};

		public:
			static LightmapNode* insertNode(Lightmap& lightmap, LightmapNode* node);

			static void updateAtlasCoordinates(std::vector<Lightmap>& lightmaps, StaticScene3D& scene);

			void buildLightmapAtlas(const StaticScene3D& staticScene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor);

		private:
			ref<RenderPass> createRenderPass(ImageFormat lightmapAtlasImageFormat) const;
			ref<Framebuffer> createFramebuffer() const;

			ref<PipelineLayout> createLightmapAtlasPipelineLayout() const;

			ref<GraphicsPipeline> createLightmapAtlasPipeline() const;

			ref<Image> createLightmapAtlasImage(const ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) const;
			ref<ImageView> createLightmapAtlasImageView();

			ref<Buffer<svec2<float32>>> createCoordinateBuffer(uint64 size) const;
			ref<Buffer<uint32>> createCoordinateIndexBuffer(uint64 size) const;

			void copyCoordinateBuffers(const StaticScene3D& staticScene) const;

		private:
			ref<RenderPass> renderPass;

			ref<Image> lightmapAtlasImage;
			ref<ImageView> lightmapAtlasImageView;

			ref<Framebuffer> framebuffer;

			ref<PipelineLayout> lightmapAtlasPipelineLayout;

			ref<GraphicsPipeline> lightmapAtlasPipeline;

			ref<Buffer<svec2<float32>>> coordinateBuffer;
			ref<Buffer<uint32>> coordinateIndexBuffer;
	};
}