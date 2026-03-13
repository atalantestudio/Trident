// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"

namespace Trident {
	class GL::Framebuffer final : public Trident::Framebuffer {
		public:
			explicit Framebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor);

		public:
			GLuint framebuffer;

			// All attachments in a framebuffer must have the same sample count.
			// For multisampled framebuffers, a separate resolve framebuffer is used.
			GLuint resolveFramebuffer;
	};

	namespace GL {
		ref<Trident::Framebuffer> createFramebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor);

		void destroyFramebuffer(const Trident::Device* device, const Trident::Framebuffer* framebuffer);
	}
}