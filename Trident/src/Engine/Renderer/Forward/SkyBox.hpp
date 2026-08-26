// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> ForwardRenderer::createSkyBoxDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::VERTEX,
			},

			// Sky box
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::FRAGMENT,
			},
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Sky box",
			#endif
		});
	}

	ref<PipelineLayout> ForwardRenderer::createSkyBoxPipelineLayout() const {
		return createPipelineLayout({
			.descriptorSetLayouts = {skyBoxDescriptorSetLayout},
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Sky box",
			#endif
		});
	}

	ref<GraphicsPipeline> ForwardRenderer::createSkyBoxPipeline() const {
		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
			},
		};

		return createGraphicsPipeline({
			.pipelineLayout = skyBoxPipelineLayout,
			.vertexInputBindings = {},
			.vertexShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::VERTEX,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Sky box vertex",
					#endif
				},
				"Forward/SkyBox.vert"
			),
			.fragmentShaderModule = loadInternalShaderModule(
				{
					.stage = ShaderStage::FRAGMENT,
					.entryPoint = "main",
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Sky box fragment",
					#endif
				},
				"Forward/SkyBox.frag"
			),
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = false,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Sky box",
			#endif
		});
	}

	ref<Image> ForwardRenderer::createSkyBoxImage() const {
		return createDynamicImage({
			.type = ImageType::TYPE_2D,
			.flags = ImageFlags::CUBE_COMPATIBLE,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {512, 512, 1},
			.mipLevelCount = 1,
			.layerCount = 6,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Sky box",
			#endif
		});
	}

	ref<ImageView> ForwardRenderer::createSkyBoxImageView() const {
		return skyBoxImage->createView({
			.type = ImageViewType::TYPE_CUBE,
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 6,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Sky box view",
			#endif
		});
	}
}