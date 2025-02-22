#pragma once

#include "ProjectA/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	class OpenGLVertexBuffer : public VertexBuffer {
		private:
			BufferLayout layout;

		public:
			OpenGLVertexBuffer(uint32 size);
			OpenGLVertexBuffer(float32* vertices, uint32 size);

			virtual ~OpenGLVertexBuffer();

			virtual const BufferLayout& getLayout() const override;

			virtual void setLayout(const BufferLayout& layout) override;

			virtual void copyData(const void* data, uint32 size) override;

			virtual void bind() const override;

			virtual void unbind() const override;
	};
}