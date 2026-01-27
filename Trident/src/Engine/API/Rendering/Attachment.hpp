#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/LoadOperation.hpp"
#include "Engine/API/Rendering/StoreOperation.hpp"

namespace Trident {
	struct Attachment {
		// The format of the image view that will be used for the attachment.
		ImageFormat format;

		// The number of samples of the image.
		// NOTE: The sample count must either be 1 or a power of 2 (up to 64).
		uint8 sampleCount : 7;

		// Specifies how the attachment's contents are treated at the beginning of the subpass where it is first used.
		LoadOperation loadOperation;

		// Specifies how the attachment's contents are treated at the end of the subpass where it is last used.
		StoreOperation storeOperation;

		// The layout the attachment image will be transitioned to when the render pass ends.
		ImageLayout finalLayout;
	};
}