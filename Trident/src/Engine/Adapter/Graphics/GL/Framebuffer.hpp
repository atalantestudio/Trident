// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"

namespace Trident {
	class GL::Framebuffer final : public Trident::Framebuffer {
		public:
			explicit Framebuffer(const FramebufferDescriptor& descriptor);

			~Framebuffer() final override;

		public:
			GLuint framebuffer;
	};
}