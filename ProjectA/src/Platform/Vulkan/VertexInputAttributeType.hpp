#include "pch.hpp"

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Buffer/VertexInputAttributeType.hpp"

namespace ProjectA {
	template<VertexInputAttributeType T>
	static VkFormat getVkFormat();

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R8_SINT>() {
		return VK_FORMAT_R8_SINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R8_UINT>() {
		return VK_FORMAT_R8_UINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R16_SINT>() {
		return VK_FORMAT_R16_SINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R16_UINT>() {
		return VK_FORMAT_R16_UINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R32_SINT>() {
		return VK_FORMAT_R32_SINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R32_UINT>() {
		return VK_FORMAT_R32_UINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R32_FLOAT>() {
		return VK_FORMAT_R32_SFLOAT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R32G32_FLOAT>() {
		return VK_FORMAT_R32G32_SFLOAT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R32G32B32_FLOAT>() {
		return VK_FORMAT_R32G32B32_SFLOAT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R32G32B32A32_FLOAT>() {
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R64_SINT>() {
		return VK_FORMAT_R64_SINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R64_UINT>() {
		return VK_FORMAT_R64_UINT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R64_FLOAT>() {
		return VK_FORMAT_R64_SFLOAT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R64G64_FLOAT>() {
		return VK_FORMAT_R64G64_SFLOAT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R64G64B64_FLOAT>() {
		return VK_FORMAT_R64G64B64_SFLOAT;
	}

	template<>
	static inline VkFormat getVkFormat<VertexInputAttributeType::R64G64B64A64_FLOAT>() {
		return VK_FORMAT_R64G64B64A64_SFLOAT;
	}
}