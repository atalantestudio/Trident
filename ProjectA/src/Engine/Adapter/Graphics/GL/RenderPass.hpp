// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class GL::RenderPass final : public ProjectA::RenderPass {
		public:
			explicit RenderPass(const RenderPassDescriptor& descriptor);

			uint32 getPresentSourceAttachmentIndex() const {
				return presentSourceAttachmentIndex;
			}

		private:
			uint32 presentSourceAttachmentIndex = max<uint32>();
	};
}