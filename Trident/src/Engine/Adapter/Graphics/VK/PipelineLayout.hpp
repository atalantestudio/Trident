// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Graphics/PipelineLayout.hpp"

namespace Trident {
	class VK::PipelineLayout final : public Trident::PipelineLayout {
		public:
			explicit PipelineLayout(const Trident::Device* device);

		public:
			VkPipelineLayout pipelineLayout;
	};

	namespace VK {
		ref<Trident::PipelineLayout> createPipelineLayout(const Trident::Device* device, const PipelineLayoutDescriptor& descriptor);

		void destroyPipelineLayout(const Trident::Device* device, const Trident::PipelineLayout* pipelineLayout);
	}
}