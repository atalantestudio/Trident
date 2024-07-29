#pragma once

#include "ProjectA/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	class OpenGLVertexBuffer : public VertexBuffer {
		private:
			BufferLayout layout;

		public:
			OpenGLVertexBuffer(float* vertices, uint32_t size);

			virtual ~OpenGLVertexBuffer();

			virtual const BufferLayout& getLayout() const override;

			virtual void setLayout(const BufferLayout& layout) override;

			virtual void bind() const override;

			virtual void unbind() const override;
	};
}