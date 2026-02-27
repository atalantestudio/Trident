// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"

namespace Trident {
	class VK::RenderPass final : public Trident::RenderPass {
		public:
			explicit RenderPass(const Trident::Device* device, const RenderPassDescriptor& descriptor);

		public:
			VkRenderPass renderPass;
			VkExtent2D surfaceExtent;
	};

	namespace VK {
		ref<Trident::RenderPass> createRenderPass(const Trident::Device* device, ref<Trident::Surface> surface, const RenderPassDescriptor& descriptor);

		void destroyRenderPass(const Trident::Device* device, const Trident::RenderPass* renderPass);
	}
}