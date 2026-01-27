// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace Trident {
	class VK::RenderPass final : public Trident::RenderPass {
		public:
			explicit RenderPass(const RenderPassDescriptor& descriptor, const VK::Surface& surface, const VK::Device& device);

			~RenderPass() final override;

		private:
			// Returns true if this attachment is a color attachment used in any subpass.
			bool isColorAttachment(uint32 attachmentIndex) const;

		public:
			VkRenderPass renderPass;
			const VK::Surface& surface;

		private:
			const VK::Device& device;
	};
}