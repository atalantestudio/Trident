// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Pipeline.hpp"
#include "Engine/API/Rendering/GraphicsPipeline.hpp"

namespace Trident {
	class VK::GraphicsPipeline final : public VK::Pipeline, public Trident::GraphicsPipeline {
		public:
			explicit GraphicsPipeline(const GraphicsPipelineDescriptor& descriptor, const VK::Device& device);

			~GraphicsPipeline() final override;

		private:
			const VK::Device& device;
	};
}