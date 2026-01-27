// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"

namespace Trident {
	class GL::RenderPass final : public Trident::RenderPass {
		public:
			explicit RenderPass(const RenderPassDescriptor& descriptor);

		public:
			uint32 presentSourceAttachmentIndex = max<uint32>();
			bool multisample = false;
	};
}