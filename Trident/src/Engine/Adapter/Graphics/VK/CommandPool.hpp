// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandPool.hpp"

namespace Trident {
	class VK::CommandPool final : public Trident::CommandPool {
		public:
			explicit CommandPool(const Trident::Device* device);

		public:
			VkCommandPool commandPool;
			VkPipelineStageFlags pipelineStageFlags;
	};

	namespace VK {
		ref<Trident::CommandPool> createCommandPool(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);

		void destroyCommandPool(const Trident::Device* device, const Trident::CommandPool* commandPool);
	}
}