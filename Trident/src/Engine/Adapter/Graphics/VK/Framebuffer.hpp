// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"

namespace Trident {
	class VK::Framebuffer final : public Trident::Framebuffer {
		public:
			explicit Framebuffer(const FramebufferDescriptor& descriptor, const VK::Device& device);

			~Framebuffer() final override;

		public:
			VkFramebuffer framebuffer;

		private:
			const VK::Device& device;
	};
}