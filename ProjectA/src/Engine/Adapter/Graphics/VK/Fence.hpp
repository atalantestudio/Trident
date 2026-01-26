// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/Fence.hpp"

namespace ProjectA {
	class VK::Fence final : public ProjectA::Fence {
		public:
			explicit Fence(const FenceDescriptor& descriptor, const VK::Device& device);

			~Fence() final override;

		public:
			VkFence fence;

		private:
			const VK::Device& device;
	};
}