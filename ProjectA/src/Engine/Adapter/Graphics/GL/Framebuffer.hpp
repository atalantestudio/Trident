// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"

namespace ProjectA {
	class GL::Framebuffer final : public ProjectA::Framebuffer {
		public:
			explicit Framebuffer(const FramebufferDescriptor& descriptor);

			~Framebuffer() final override;

		public:
			GLuint framebuffer;
	};
}