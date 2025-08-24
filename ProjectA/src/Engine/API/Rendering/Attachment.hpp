#pragma once

#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/LoadOperation.hpp"
#include "Engine/API/Rendering/StoreOperation.hpp"

namespace ProjectA {
	struct ColorAttachment {
		// Must be a color format.
		ImageFormat format;

		// The number of samples of the attachment. It must be either 1 or a power of 2 up to 64.
		uint8 sampleCount : 7;

		// Specifies how the attachment's contents are treated at the beginning of the subpass.
		LoadOperation loadOperation;

		// Specifies how the attachment's contents are treated at the end of the subpass.
		StoreOperation storeOperation;

		ImageLayout finalImageLayout;
	};

	struct DepthStencilAttachment {
		// TODO(review): Use a different ImageFormat enum to automatically restrict image formats?
		// Must be a depth and/or depth stencil format.
		ImageFormat format;

		// The number of samples of the attachment. It must be either 1 or a power of 2 up to 64.
		uint8 sampleCount : 7;

		// Specifies how the attachment's contents are treated at the beginning of the subpass.
		LoadOperation loadOperation;
	};
}