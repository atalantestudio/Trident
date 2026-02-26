// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"

namespace Trident {
	class VK::Framebuffer final : public Trident::Framebuffer {
		public:
			explicit Framebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor);

		public:
			VkFramebuffer framebuffer;
	};

	namespace VK {
		ref<Trident::Framebuffer> createFramebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor);

		void destroyFramebuffer(const Trident::Device* device, const Trident::Framebuffer* framebuffer);
	}
}