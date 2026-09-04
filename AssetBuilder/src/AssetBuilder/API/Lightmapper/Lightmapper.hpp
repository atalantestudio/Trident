// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Scene/Scene3D.hpp"

#include "Engine/API/Graphics/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/EXT_hardware_ray_tracing.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct LightmapperDescriptor {
		view<BuildVertex3D> vertices;
		view<svec2<float32>> lightmapCoordinates;
		view<uint32> indices;
		view<LightmapMaterial> materials;
		view<uint32> emissiveTriangleIndices;
		const BuildScene3D* scene;
		Scene3DDescriptor sceneDescriptor;
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

		ref<Buffer> materialBuffer;
		ref<Buffer> emissiveTriangleIndexBuffer;
		ref<Buffer> lightmapAtlasStagingBuffer;
	};

	// TODO(application/destruction-leaks)
	class Lightmapper {
		private:
			struct Arguments {
				Extent2D atlasExtent;
				uint32 triangleCount;
				uint32 emissiveTriangleCount;
				svec3<float32> skyColor;
				uint32 _1;
				svec3<float32> sunDirection;
				uint32 _2;
				svec3<float32> sunColor;
				float32 sunIntensity;
			};

		public:
			static const GraphicsAdapter* adapter;

		public:
			explicit Lightmapper() {}

			~Lightmapper();

			void generateAtlas(ImageAssetDescriptor& imageAssetDescriptor, const LightmapperDescriptor& descriptor);

		private:
			void initialize();

			// TODO(asset-builder/scene/lightmapper): Move inside the engine.
			ref<ShaderModule> loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const;
			ref<ShaderModule> loadCompiledShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const;

			ref<RenderPass> createGBufferRenderPass() const;
			ref<RenderPass> createDilationRenderPass() const;

			ref<Image> createGBufferPositionImage() const;
			ref<Image> createGBufferSmoothPositionImage() const;
			ref<Image> createGBufferTriangleNormalImage() const;
			ref<Image> createGBufferVertexNormalImage() const;
			ref<Image> createIntermediateLightmapAtlasImage() const;
			ref<Image> createLightmapAtlasImage() const;

			ref<ImageView> createGBufferImageView(ref<Image> image, view<char8> name) const;
			ref<ImageView> createIntermediateLightmapAtlasImageView();
			ref<ImageView> createLightmapAtlasImageView();

			ref<Framebuffer> createGBufferFramebuffer() const;
			ref<Framebuffer> createDilationFramebuffer() const;

			ref<DescriptorSetLayout> createGBufferDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createRayTracingDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createPostProcessingDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createDilationDescriptorSetLayout() const;

			ref<GraphicsPipeline> createGBufferPipeline() const;
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

			void buildAccelerationStructures(const BuildScene3D& scene);

			void encodeOneTimeCommands(const LightmapperDescriptor& descriptor) const;

			void generateLightmaps(uint32 vertexCount) const;

		private:
			bool initialized = false;
			ImageFormat atlasFormat;
			Extent3D atlasExtent;
			ImageAssetDescriptor imageAssetDescriptor;
			Scene3DDescriptor sceneDescriptor;

			ref<Instance> instance;
			ref<PhysicalDevice> physicalDevice;
			const PhysicalDeviceLimits* limits;
			ref<ExtendedDevice<EXT_hardware_ray_tracing>> device;

			ref<RenderPass> gBufferRenderPass;
			ref<RenderPass> dilationRenderPass;

			union {
				struct {
					ref<DescriptorSetLayout> gBufferDescriptorSetLayout;
					ref<DescriptorSetLayout> rayTracingDescriptorSetLayout;
					ref<DescriptorSetLayout> postProcessingDescriptorSetLayout;
					ref<DescriptorSetLayout> dilationDescriptorSetLayout;
				};

				ref<DescriptorSetLayout> descriptorSetLayouts[4];
			};

			sequence<ref<DescriptorSet>> descriptorSets;

			ref<PipelineLayout> gBufferPipelineLayout;
			ref<PipelineLayout> rayTracingPipelineLayout;
			ref<PipelineLayout> postProcessingPipelineLayout;
			ref<PipelineLayout> dilationPipelineLayout;

			ref<GraphicsPipeline> gBufferPipeline;
			ref<RayTracingPipeline> rayTracingPipeline;
			ref<ComputePipeline> intermediateDilationPipeline;
			ref<ComputePipeline> horizontalBilateralFilterPipeline;
			ref<ComputePipeline> verticalBilateralFilterPipeline;
			ref<GraphicsPipeline> dilationPipeline;

			LightmapperContext context;

			ref<Buffer> argumentBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> accelerationStructureBuffer;
			ref<Buffer> shaderBindingTableBuffer;
			ref<AccelerationStructure> bottomLevelAccelerationStructure;
			ref<AccelerationStructure> topLevelAccelerationStructure;
			ref<Sampler> intermediateLightmapAtlasSampler;

			DeviceAddressRegion rayGenerationShaderBindingTable;
			DeviceAddressRegion rayMissShaderBindingTable;
			DeviceAddressRegion rayHitShaderBindingTable;
			DeviceAddressRegion rayCallableShaderBindingTable;
	};
}