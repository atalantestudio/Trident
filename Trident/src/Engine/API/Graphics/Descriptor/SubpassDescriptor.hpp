// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Attachment.hpp"
#include "Engine/API/Graphics/AttachmentReference.hpp"

namespace Trident {
	struct SubpassDescriptor {
		sequence<AttachmentReference> colorAttachmentReferences;
		AttachmentReference depthStencilAttachmentReference = {AttachmentReference::UNUSED_ATTACHMENT_INDEX, ImageLayout::UNDEFINED};
		sequence<AttachmentReference> inputAttachmentReferences;
	};
}