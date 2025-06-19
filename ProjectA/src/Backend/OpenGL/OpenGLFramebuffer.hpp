#pragma once

#include "Engine/API/Rendering/Framebuffer.hpp"
#include "Engine/API/ObjectWrapper.hpp"

namespace ProjectA {
	class OpenGLFramebuffer : public Framebuffer, public ObjectWrapper<GLuint> {
		public:
			OpenGLFramebuffer(const FramebufferDescriptor& descriptor);

			~OpenGLFramebuffer() final override;
	};
}