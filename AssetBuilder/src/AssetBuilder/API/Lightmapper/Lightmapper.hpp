// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Map/Scene3D.hpp"

#include "Engine/API/Graphics/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/EXT_hardware_ray_tracing.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct LightmapperDescriptor {
		BuildScene3D* scene;
		view<uint32> emissiveTriangleIndices;
		view<BuildTriangle3D> triangles;
		view<LightmapMaterial> materials;
	};

	struct LightmapperContext {
		ref<ImageView> gBufferPositionImageView;
		ref<ImageView> gBufferSmoothPositionImageView;
		ref<ImageView> gBufferTriangleNormalImageView;
		ref<ImageView> gBufferVertexNormalImageView;
		ref<ImageView> intermediateLightmapAtlasImageView;
		ref<ImageView> lightmapAtlasImageView;

		ref<Image> gBufferPositionImage;
		ref<Image> gBufferSmoothPositionImage;
		ref<Image> gBufferTriangleNormalImage;
		ref<Image> gBufferVertexNormalImage;
		ref<Image> intermediateLightmapAtlasImage;
		ref<Image> lightmapAtlasImage;

		ref<Framebuffer> gBufferFramebuffer;
		ref<Framebuffer> dilationFramebuffer;
	};

	// TODO(application/destruction-leaks)
	class Lightmapper {
		private:
			struct Arguments {
				Extent2D atlasSize;
				uint32 emissiveTriangleCount;
			};

			struct LightmapVertex {
				alignas(16) svec3<float32> position;
				alignas(16) svec3<float32> normal;
			};

			struct LightmapInstance {
				float32 transform[3][4];
				uint32 geometryTriangleOffset;
				uint32 deduplicatedTriangleOffset;
				uint32 lightmapCoordinateOffset;
			};

		public:
			explicit Lightmapper() {}

			~Lightmapper();

			void initialize(const GraphicsAdapter* adapter);

			void generateLightmapAtlas(const LightmapperDescriptor& descriptor);

		private:
			// TODO(asset-builder/scene/lightmapper): Move inside the engine.
			ref<ShaderModule> loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const;

			ref<RenderPass> createGBufferRenderPass() const;
			ref<RenderPass> createDilationRenderPass() const;

			ref<Image> createEnvironmentMapImage(Extent3D extent) const;
			ref<Image> createGBufferPositionImage() const;
			ref<Image> createGBufferSmoothPositionImage() const;
			ref<Image> createGBufferTriangleNormalImage() const;
			ref<Image> createGBufferVertexNormalImage() const;
			ref<Image> createIntermediateLightmapAtlasImage() const;
			ref<Image> createLightmapAtlasImage() const;

			ref<ImageView> createEnvironmentMapImageView() const;
			ref<ImageView> createGBufferImageView(ref<Image> image, view<char8> name) const;
			ref<ImageView> createIntermediateLightmapAtlasImageView();
			ref<ImageView> createLightmapAtlasImageView();

			ref<Framebuffer> createGBufferFramebuffer() const;
			ref<Framebuffer> createDilationFramebuffer() const;

			ref<DescriptorSetLayout> createGBufferDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createRayTracingDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createPostProcessingDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createDilationDescriptorSetLayout() const;

			ref<PipelineLayout> createGBufferPipelineLayout() const;
			ref<PipelineLayout> createRayTracingPipelineLayout() const;
			ref<PipelineLayout> createPostProcessingPipelineLayout() const;
			ref<PipelineLayout> createDilationPipelineLayout() const;

			ref<GraphicsPipeline> createGBufferPipeline(PolygonMode polygonMode) const;
			ref<RayTracingPipeline> createRayTracingPipeline() const;
			ref<ComputePipeline> createIntermediateDilationPipeline() const;
			ref<ComputePipeline> createHorizontalBilateralFilterPipeline() const;
			ref<ComputePipeline> createVerticalBilateralFilterPipeline() const;
			ref<GraphicsPipeline> createDilationPipeline() const;

			void createShaderBindingTableBuffer();
			ref<Buffer> createArgumentBuffer() const;
			ref<Buffer> createVertexBuffer(uint64 vertexCount) const;
			ref<Buffer> createIndexBuffer(uint64 indexCount) const;
			ref<Buffer> createMaterialBuffer(uint64 materialCount) const;
			ref<Buffer> createEmissiveTriangleIndexBuffer(uint64 emissiveTriangleIndexCount) const;

			ref<Sampler> createIntermediateLightmapAtlasSampler() const;
			ref<Sampler> createEnvironmentMapSampler() const;

			void encodeOneTimeCommands(const LightmapperDescriptor& descriptor) const;
			void createBottomLevelAccelerationStructures(const BuildScene3D& scene);
			void createTopLevelAccelerationStructure(const BuildScene3D& scene);
			void updateDescriptorSets() const;
			void generateLightmaps(const BuildScene3D& scene) const;

			void encodeGBufferCommands(const BuildScene3D& scene) const;

		private:
			const GraphicsAdapter* adapter = nullptr;
			bool initialized = false;
			ImageFormat atlasFormat;
			Extent3D atlasExtent;
			ImageAssetDescriptor* imageAssetDescriptor = nullptr;
			MapDescriptor mapDescriptor;

			ref<Instance> instance;
			ref<PhysicalDevice> physicalDevice;
			const PhysicalDeviceLimits* limits;
			ref<ExtendedDevice<EXT_hardware_ray_tracing>> device;
			ref<Queue> queue;
			ref<ExtendedCommandEncoder<EXT_hardware_ray_tracing>> commandEncoder;

			ref<RenderPass> gBufferRenderPass;
			ref<RenderPass> dilationRenderPass;

			union {
				struct {
					ref<DescriptorSetLayout> gBufferDescriptorSetLayout;
					ref<DescriptorSetLayout> rayTracingDescriptorSetLayout;
					ref<DescriptorSetLayout> postProcessingDescriptorSetLayout;
					ref<DescriptorSetLayout> dilationDescriptorSetLayout;
				};

				// TODO(asset-builder/lightmapper): Why these need to be default-initialized when the buffer unions in the graphics samples don't need to be?
				ref<DescriptorSetLayout> descriptorSetLayouts[4]{};
			};

			sequence<ref<DescriptorSet>> descriptorSets;

			ref<PipelineLayout> gBufferPipelineLayout;
			ref<PipelineLayout> rayTracingPipelineLayout;
			ref<PipelineLayout> postProcessingPipelineLayout;
			ref<PipelineLayout> dilationPipelineLayout;

			ref<GraphicsPipeline> gBufferPipelines[2];
			ref<RayTracingPipeline> rayTracingPipeline;
			ref<ComputePipeline> intermediateDilationPipeline;
			ref<ComputePipeline> horizontalBilateralFilterPipeline;
			ref<ComputePipeline> verticalBilateralFilterPipeline;
			ref<GraphicsPipeline> dilationPipeline;

			LightmapperContext context;

			ref<Buffer> argumentBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> lightmapCoordinateBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> emissiveTriangleIndexBuffer;
			ref<Buffer> triangleBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> materialBuffer;

			ref<Buffer> tlasInstanceBuffer;
			ref<Buffer> blasBuffer;
			ref<Buffer> tlasBuffer;
			ref<Buffer> blasScratchBuffer;
			ref<Buffer> tlasScratchBuffer;
			ref<Buffer> shaderBindingTableBuffer;

			ref<Buffer> lightmapAtlasStagingBuffer;

			sequence<ref<AccelerationStructure>> blas;
			ref<AccelerationStructure> tlas;

			sequence<BaseBufferView> blasScratchViews;
			BaseBufferView tlasScratchView;

			ref<Image> environmentMapImage;

			ref<ImageView> environmentMapImageView;

			ref<Sampler> environmentMapSampler;
			ref<Sampler> intermediateLightmapAtlasSampler;

			DeviceAddressRegion rayGenerationShaderBindingTable;
			DeviceAddressRegion rayMissShaderBindingTable;
			DeviceAddressRegion rayHitShaderBindingTable;
			DeviceAddressRegion rayCallableShaderBindingTable;
	};
}