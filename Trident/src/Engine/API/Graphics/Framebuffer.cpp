// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Framebuffer.hpp"

namespace Trident {
	Framebuffer::Framebuffer(const Device* device, const FramebufferDescriptor& descriptor) :
		device(device),
		renderPass(descriptor.renderPass),
		imageViews(descriptor.imageViews),
		extent(descriptor.extent)
	{}

	Framebuffer::~Framebuffer() {
		device->adapter->destroyFramebufferImplementation(device, this);
	}

	ref<RenderPass> Framebuffer::getRenderPass() const {
		return renderPass;
	}

	const sequence<ref<ImageView>>& Framebuffer::getImageViews() const {
		return imageViews;
	}

	Extent2D Framebuffer::getExtent() const {
		return extent;
	}
}