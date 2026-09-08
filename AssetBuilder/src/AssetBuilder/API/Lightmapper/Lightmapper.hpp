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

			struct LightmapVertex {
				svec3<float32> position;
				uint32 materialIndex;
				alignas(16) svec3<float32> normal;
			};

			struct LightmapInstance {
				float32 transform[3][4];
				uint32 geometryTriangleOffset;
				uint32 geometryVertexOffset;
				uint32 lightmapCoordinateOffset;
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

			void encodeOneTimeCommands(const LightmapperDescriptor& descriptor) const;
			void createBottomLevelAccelerationStructures(const BuildScene3D& scene);
			void createTopLevelAccelerationStructure(const BuildScene3D& scene);
			void updateDescriptorSets() const;
			void generateLightmaps(const BuildScene3D& scene, uint32 vertexCount) const;

			void encodeGBufferCommands(const BuildScene3D& scene) const;

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

			ref<Buffer> vertexBuffer;
			ref<Buffer> lightmapCoordinateBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> tlasInstanceBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> argumentBuffer;
			ref<Buffer> blasBuffer;
			ref<Buffer> tlasBuffer;
			ref<Buffer> blasScratchBuffer;
			ref<Buffer> tlasScratchBuffer;
			ref<Buffer> shaderBindingTableBuffer;

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