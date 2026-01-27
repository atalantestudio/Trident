// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"

namespace Trident {
	class DX12::Framebuffer final : public Trident::Framebuffer {
		public:
			explicit Framebuffer(const FramebufferDescriptor& descriptor);

		public:
			sequence<ref<Trident::ImageView>> imageViews;
	};
}