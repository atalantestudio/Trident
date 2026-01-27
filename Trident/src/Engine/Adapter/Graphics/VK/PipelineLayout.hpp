// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/PipelineLayout.hpp"

namespace Trident {
	class VK::PipelineLayout final : public Trident::PipelineLayout {
		public:
			explicit PipelineLayout(const PipelineLayoutDescriptor& descriptor, const VK::Device& device);

			~PipelineLayout() final override;

		public:
			VkPipelineLayout pipelineLayout;

		private:
			const VK::Device& device;
	};
}