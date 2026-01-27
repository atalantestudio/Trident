// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageLayout.hpp"

namespace Trident {
	struct AttachmentReference {
		public:
			static constexpr uint32 UNUSED_ATTACHMENT_INDEX = max<uint32>();

		public:
			// Index of the attachment in the render pass.
			// Use UNUSED_ATTACHMENT_INDEX if the attachment is not used during the subpass.
			uint32 index;

			// Layout of the attachment during the subpass.
			ImageLayout layout;
	};
}