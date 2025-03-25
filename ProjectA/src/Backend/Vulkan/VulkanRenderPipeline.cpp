#include "pch.hpp"

#include "Backend/Vulkan/CullMode.hpp"
#include "Backend/Vulkan/FrontFace.hpp"
#include "Backend/Vulkan/InputRate.hpp"
#include "Backend/Vulkan/VertexInputAttributeType.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanRenderPipeline::VulkanRenderPipeline(VkDevice device, const RenderPipelineDescriptor& descriptor) :
		device(device),
		pipelineLayout(static_cast<const VulkanPipelineLayout*>(descriptor.pipelineLayout.get()))
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanRenderPipeline]");

		chain<VkVertexInputBindingDescription> vertexInputBindingDescriptions;

		for (const VertexBinding& binding : descriptor.vertexBufferLayout.vertexBindings) {
			VkVertexInputBindingDescription vertexInputBindingDescription{};
			vertexInputBindingDescription.binding = binding.index;
			vertexInputBindingDescription.stride = binding.stride;
			vertexInputBindingDescription.inputRate = getVkVertexInputRate(binding.inputRate);

			vertexInputBindingDescriptions.emplace_back(vertexInputBindingDescription);
		}

		chain<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

		for (const VertexAttribute& attribute : descriptor.vertexBufferLayout.vertexAttributes) {
			VkVertexInputAttributeDescription vertexInputAttributeDescription{};
			vertexInputAttributeDescription.location = attribute.location;
			vertexInputAttributeDescription.binding = attribute.binding;
			vertexInputAttributeDescription.format = getVkFormat(attribute.type);
			vertexInputAttributeDescription.offset = attribute.offset;

			vertexInputAttributeDescriptions.emplace_back(vertexInputAttributeDescription);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescriptions.size());
		vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vertexInputAttributeDescriptions.size());
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

		static constexpr VkPrimitiveTopology topologies[] = {
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,

			// Only if the VK_KHR_portability_subset extension is NOT enabled.
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,

			// Only if the geometryShader feature is enabled.
			// VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,

			// Only if the tessellationShader feature is enabled.
			// VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = nullptr;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = topologies[static_cast<uint32>(descriptor.primitiveTopology)];
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		const VulkanSwapchain* swapchain = static_cast<const VulkanSwapchain*>(descriptor.swapchain.get());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float32>(swapchain->imageExtent.width);
		viewport.height = static_cast<float32>(swapchain->imageExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = swapchain->imageExtent;

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
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.cullMode = getVkCullMode(descriptor.cullMode);
		rasterizationStateCreateInfo.frontFace = getVkFrontFace(descriptor.frontFace);
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = 1.0f;

		const VkSampleMask sampleMask = 1;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = nullptr;
		multisampleStateCreateInfo.flags = 0;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.minSampleShading = 0.0f;
		multisampleStateCreateInfo.pSampleMask = &sampleMask;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
		colorBlendAttachmentState.blendEnable = VK_FALSE;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = nullptr;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		const chain<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		const chain<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = {
			static_cast<const VulkanShaderModule*>(descriptor.vertexShaderModule.get())->object,
			static_cast<const VulkanShaderModule*>(descriptor.fragmentShaderModule.get())->object,
		};

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast<uint32>(shaderStageCreateInfos.size());
		graphicsPipelineCreateInfo.pStages = shaderStageCreateInfos.data();
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = static_cast<const VulkanPipelineLayout*>(descriptor.pipelineLayout.get())->object;
		graphicsPipelineCreateInfo.renderPass = static_cast<const VulkanRenderPass*>(descriptor.renderPass.get())->object;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan render pipeline.");
		}

		// TODO: Shader modules can be destroyed here.
	}

	VulkanRenderPipeline::~VulkanRenderPipeline() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanRenderPipeline]");

		vkDestroyPipeline(device, object, nullptr);
	}
}