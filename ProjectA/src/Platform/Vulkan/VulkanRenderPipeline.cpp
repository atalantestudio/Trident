#include "pch.hpp"

#include "Platform/Vulkan/VulkanRenderPipeline.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanRenderPipeline::VulkanRenderPipeline(const VkDevice device, const VkRenderPass renderPass, const RenderPipelineDescriptor& descriptor) :
		device(device)
	{
		const std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = {
			reinterpret_cast<const VulkanShaderModule*>(&descriptor.vertexShaderModule)->object,
			reinterpret_cast<const VulkanShaderModule*>(&descriptor.fragmentShaderModule)->object,
		};

		//const VkVertexInputBindingDescription bindingDescription;
		//const std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		//vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		//vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
		//vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDescriptions.size());
		//vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		static constexpr VkPrimitiveTopology topologies[] = {
			VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
			VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,

			// Only if the VK_KHR_portability_subset extension is NOT enabled
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,

			// Only if the geometryShader feature is enabled
			// VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
			// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,

			// Only if the tessellationShader feature is enabled
			// VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
		inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyCreateInfo.pNext = nullptr;
		inputAssemblyCreateInfo.flags = 0;
		inputAssemblyCreateInfo.topology = topologies[static_cast<uint32>(descriptor.primitiveTopology)];
		inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast<uint32>(shaderStageCreateInfos.size());
		graphicsPipelineCreateInfo.pStages = shaderStageCreateInfos.data();
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		//graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		//graphicsPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
		//graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
		//graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		//graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		//graphicsPipelineCreateInfo.layout = layout;
		graphicsPipelineCreateInfo.renderPass = renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan render pipeline.");
		}

		// TODO: Destroy shader modules here.
	}

	VulkanRenderPipeline::~VulkanRenderPipeline() {
		vkDestroyPipeline(device, object, nullptr);
	}

	// --- TODO: Remove these implementations ---

	void VulkanRenderPipeline::bind() const {}

	void VulkanRenderPipeline::unbind() const {}

	const std::vector<ref<VertexBuffer>>& VulkanRenderPipeline::getVertexBuffers() const {
		ENGINE_ASSERT(false, "TODO");

		std::vector<ref<VertexBuffer>> a;

		return a;
	}

	const ref<IndexBuffer>& VulkanRenderPipeline::getIndexBuffer() const {
		ENGINE_ASSERT(false, "TODO");

		return 0;
	}

	void VulkanRenderPipeline::addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) {}

	void VulkanRenderPipeline::setIndexBuffer(const ref<IndexBuffer>& indexBuffer) {}

	void VulkanRenderPipeline::setUniform(const std::string& name, int32 value) const {}
	void VulkanRenderPipeline::setUniform(const std::string& name, const int32* value, uint32 count) const {}
	void VulkanRenderPipeline::setUniform(const std::string& name, float32 value) const {}
	void VulkanRenderPipeline::setUniform(const std::string& name, const vec2<float32>& value) const {}
	void VulkanRenderPipeline::setUniform(const std::string& name, const vec3<float32>& value) const {}
	void VulkanRenderPipeline::setUniform(const std::string& name, const vec4<float32>& value) const {}
	void VulkanRenderPipeline::setUniform(const std::string& name, const mat4x4<float32>& value) const {}
}