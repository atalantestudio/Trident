#pragma once

#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/LoadOperation.hpp"
#include "Engine/API/Rendering/StoreOperation.hpp"

namespace ProjectA {
	struct ColorAttachment {
		ImageDescriptor imageDescriptor;

		// Specifies how the attachment's contents are treated at the beginning of the subpass.
		LoadOperation loadOperation;

		// Specifies how the attachment's contents are treated at the end of the subpass.
		StoreOperation storeOperation;
	};

	struct DepthStencilAttachment {
		bool enabled = false;

		// The number of samples of the attachment. It must be a power of 2.
		uint8 sampleCount;

		// Specifies how the attachment's contents are treated at the beginning of the subpass.
		LoadOperation loadOperation;
	};
}