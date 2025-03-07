#pragma once

#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	class OpenGLRenderPass : public RenderPass {
		public:
			OpenGLRenderPass(const RenderPassDescriptor& descriptor);
	};
}