// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"

namespace Trident {
	DX12::Framebuffer::Framebuffer(const FramebufferDescriptor& descriptor) :
		Trident::Framebuffer(descriptor),
		imageViews(descriptor.imageViews)
	{}
}