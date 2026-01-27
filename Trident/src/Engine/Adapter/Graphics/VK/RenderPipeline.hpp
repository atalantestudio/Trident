// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Pipeline.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace Trident {
	class VK::RenderPipeline final : public VK::Pipeline, public Trident::RenderPipeline {
		public:
			explicit RenderPipeline(const RenderPipelineDescriptor& descriptor, const VK::Device& device);

			~RenderPipeline() final override;

		private:
			const VK::Device& device;
	};
}