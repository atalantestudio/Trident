#pragma once

#include "ProjectA/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	class OpenGLVertexBuffer : public VertexBuffer {
		public:
			OpenGLVertexBuffer(float* vertices, uint32_t size);

			virtual ~OpenGLVertexBuffer();

			virtual void bind() const override;

			virtual void unbind() const override;
	};
}