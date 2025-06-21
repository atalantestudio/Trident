#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"
#include "Engine/API/ObjectWrapper.hpp"

namespace ProjectA {
	class OpenGLFramebuffer final : public Framebuffer, public ObjectWrapper<GLuint> {
		public:
			explicit OpenGLFramebuffer(const FramebufferDescriptor& descriptor);

			~OpenGLFramebuffer() final override;
	};
}