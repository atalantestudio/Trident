#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class OpenGLRenderPass final : public RenderPass {
		public:
			explicit OpenGLRenderPass(const RenderPassDescriptor& descriptor);

			uint32 getPresentSourceAttachmentIndex() const {
				return presentSourceAttachmentIndex;
			}

		private:
			uint32 presentSourceAttachmentIndex = max<uint32>();
	};
}