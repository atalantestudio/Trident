// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class GL::RenderPass final : public ProjectA::RenderPass {
		public:
			explicit RenderPass(const RenderPassDescriptor& descriptor);

		public:
			uint32 presentSourceAttachmentIndex = max<uint32>();
			bool multisample = false;
	};
}