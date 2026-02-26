// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Pipeline.hpp"
#include "Engine/API/Rendering/GraphicsPipeline.hpp"

namespace Trident {
	class VK::GraphicsPipeline final : public Trident::GraphicsPipeline {
		public:
			explicit GraphicsPipeline(const Trident::Device* device);

		public:
			VkPipeline pipeline;
			// VkPipelineBindPoint bindPoint;
	};

	namespace VK {
		ref<Trident::GraphicsPipeline> createGraphicsPipeline(const Trident::Device* device, const GraphicsPipelineDescriptor& descriptor);

		void destroyGraphicsPipeline(const Trident::Device* device, const Trident::GraphicsPipeline* graphicsPipeline);
	}
}