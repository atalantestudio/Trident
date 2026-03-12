// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Framebuffer.hpp"

namespace Trident {
	DX12::Framebuffer::Framebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) :
		Trident::Framebuffer(device, descriptor),
		imageViews(descriptor.imageViews)
	{}

	ref<Trident::Framebuffer> DX12::createFramebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) {
		return createRef<DX12::Framebuffer>(device, descriptor);
	}

	void DX12::destroyFramebuffer(const Trident::Device* device, const Trident::Framebuffer* framebuffer) {}
}