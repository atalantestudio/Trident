// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/BlendFactor.hpp"
#include "Engine/Adapter/Graphics/VK/CullMode.hpp"
#include "Engine/Adapter/Graphics/VK/DynamicState.hpp"
#include "Engine/Adapter/Graphics/VK/FrontFace.hpp"
#include "Engine/Adapter/Graphics/VK/InputRate.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/PolygonMode.hpp"
#include "Engine/Adapter/Graphics/VK/PrimitiveTopology.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/VertexInputAttributeType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::RenderPipeline::RenderPipeline(const RenderPipelineDescriptor& descriptor, const VK::Device& device) :
		Trident::RenderPipeline(descriptor),
		VK::Pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static constexpr VkSampleMask sampleMask = max<VkSampleMask>();

		const ref<Subpass> subpass = descriptor.renderPass->getSubpass(descriptor.subpassIndex);
		const ref<VK::RenderPass> renderPass = std::static_pointer_cast<VK::RenderPass>(descriptor.renderPass);

		VkResult result;
		std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		uint32 location = 0;

		for (uint32 vertexInputBindingIndex = 0; vertexInputBindingIndex < descriptor.vertexInputBindings.count(); vertexInputBindingIndex += 1) {
			const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];

			VkVertexInputBindingDescription vertexInputBindingDescription{};
			vertexInputBindingDescription.binding = vertexInputBindingIndex;
			vertexInputBindingDescription.stride = vertexInputBinding.getStride();
			vertexInputBindingDescription.inputRate = getVertexInputRate(vertexInputBinding.getInputRate());

			const std::vector<VertexInputAttribute>& attributes = vertexInputBinding.getAttributes();

			for (const VertexInputAttribute& attribute : attributes) {
				VkVertexInputAttributeDescription vertexInputAttributeDescription{};
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = vertexInputBindingIndex;
				vertexInputAttributeDescription.format = getFormat(attribute.type);
				vertexInputAttributeDescription.offset = attribute.offset;

				vertexInputAttributeDescriptions.emplace_back(vertexInputAttributeDescription);

				location += 1;
			}

			vertexInputBindingDescriptions.emplace_back(vertexInputBindingDescription);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.size());
		vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.size());
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = nullptr;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = getPrimitiveTopology(descriptor.primitiveTopology);
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float32>(renderPass->surface.window.getSize()[0]);
		viewport.height = static_cast<float32>(renderPass->surface.window.getSize()[1]);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent.width = renderPass->surface.window.getSize()[0];
		scissor.extent.height = renderPass->surface.window.getSize()[1];

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pNext = nullptr;
		viewportStateCreateInfo.flags = 0;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.pNext = nullptr;
		rasterizationStateCreateInfo.flags = 0;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = getPolygonMode(descriptor.polygonMode);
		rasterizationStateCreateInfo.cullMode = getCullMode(descriptor.cullMode);
		rasterizationStateCreateInfo.frontFace = getFrontFace(descriptor.frontFace);
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = descriptor.lineWidth;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = nullptr;
		multisampleStateCreateInfo.flags = 0;
		multisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(subpass->getRasterizationSampleCount());

		if (descriptor.minSampleShading == 0.0f) {
			multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
			multisampleStateCreateInfo.minSampleShading = 0.0f;
		}
		else {
			multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
			multisampleStateCreateInfo.minSampleShading = descriptor.minSampleShading;
		}

		multisampleStateCreateInfo.pSampleMask = &sampleMask;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		sequence<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(descriptor.colorBlendState.attachments.count());
		uint32 colorBlendAttachmentIndex = 0;

		while (colorBlendAttachmentIndex < colorBlendAttachmentStates.count()) {
			const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[colorBlendAttachmentIndex];

			VkPipelineColorBlendAttachmentState& colorBlendAttachmentState = colorBlendAttachmentStates[colorBlendAttachmentIndex];
			colorBlendAttachmentState.blendEnable = colorBlendAttachment.blend;
			colorBlendAttachmentState.srcColorBlendFactor = getBlendFactor(colorBlendAttachment.srcColorBlendFactor);
			colorBlendAttachmentState.dstColorBlendFactor = getBlendFactor(colorBlendAttachment.dstColorBlendFactor);
			colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			colorBlendAttachmentIndex += 1;
		}

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = nullptr;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
		colorBlendStateCreateInfo.attachmentCount = static_cast<uint32>(colorBlendAttachmentStates.count());
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentStates[0];
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		const sequence<VkDynamicState> dynamicStates = getDynamicStates(descriptor.dynamicState);

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32>(dynamicStates.count());
		dynamicStateCreateInfo.pDynamicStates = &dynamicStates[0];

		const sequence<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = {
			std::static_pointer_cast<VK::ShaderModule>(descriptor.vertexShaderModule)->createInfo,
			std::static_pointer_cast<VK::ShaderModule>(descriptor.fragmentShaderModule)->createInfo,
		};

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast<uint32>(shaderStageCreateInfos.count());
		graphicsPipelineCreateInfo.pStages = &shaderStageCreateInfos[0];
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;

		if (subpass->hasDepthStencilAttachmentReference()) {
			VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
			depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilStateCreateInfo.pNext = nullptr;
			depthStencilStateCreateInfo.flags = 0;
			depthStencilStateCreateInfo.depthTestEnable = descriptor.depthStencilState.depthTest;

			if (descriptor.depthStencilState.depthTest) {
				depthStencilStateCreateInfo.depthWriteEnable = descriptor.depthStencilState.depthWrite;
				depthStencilStateCreateInfo.depthCompareOp = static_cast<VkCompareOp>(descriptor.depthStencilState.compareOperation);
			}
			else {
				depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
				depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
			}

			depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
			depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
			depthStencilStateCreateInfo.front.failOp = VK_STENCIL_OP_KEEP;
			depthStencilStateCreateInfo.front.passOp = VK_STENCIL_OP_KEEP;
			depthStencilStateCreateInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
			depthStencilStateCreateInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilStateCreateInfo.front.compareMask = 0;
			depthStencilStateCreateInfo.front.writeMask = 0;
			depthStencilStateCreateInfo.front.reference = 0;
			depthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
			depthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
			depthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
			depthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilStateCreateInfo.back.compareMask = 0;
			depthStencilStateCreateInfo.back.writeMask = 0;
			depthStencilStateCreateInfo.back.reference = 0;
			depthStencilStateCreateInfo.minDepthBounds = 0.0f;
			depthStencilStateCreateInfo.maxDepthBounds = 1.0f;

			graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
		}
		else {
			graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		}

		graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = std::static_pointer_cast<VK::PipelineLayout>(descriptor.pipelineLayout)->pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = renderPass->renderPass;
		graphicsPipelineCreateInfo.subpass = descriptor.subpassIndex;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		result = vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline ([]).", string_VkResult(result));

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
				debugUtilsObjectNameInfo.objectHandle = (uint64) pipeline;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = vkSetDebugUtilsObjectNameEXT(device.device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif
	}

	VK::RenderPipeline::~RenderPipeline() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyPipeline(device.device, pipeline, nullptr);
	}
}