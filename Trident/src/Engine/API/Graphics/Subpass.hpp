// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/AttachmentReference.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"

namespace Trident {
	struct Subpass {
		sequence<AttachmentReference> colorAttachmentReferences;
		AttachmentReference depthStencilAttachmentReference = {AttachmentReference::UNUSED_ATTACHMENT_INDEX, ImageLayout::UNDEFINED};
		sequence<AttachmentReference> inputAttachmentReferences;

		// Automatically set when creating the render pass.
		uint8 rasterizationSampleCount = 0;
	};
}