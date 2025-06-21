#pragma once

#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class OpenGLRenderPass final : public RenderPass {
		public:
			explicit OpenGLRenderPass(const RenderPassDescriptor& descriptor);
	};
}