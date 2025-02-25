#pragma once

#include "Engine/Renderer/Buffer/VertexBuffer.hpp"

namespace ProjectA {
	class OpenGLVertexBuffer : public VertexBuffer {
		public:
			OpenGLVertexBuffer(uint32 size);
			OpenGLVertexBuffer(float32* vertices, uint32 size);

			~OpenGLVertexBuffer();

			const BufferLayout& getLayout() const override;

			void setLayout(const BufferLayout& layout) override;

			void copyData(const void* data, uint32 size) override;

			// TODO: Remove
			void bind() const override;
			void unbind() const override;

		private:
			BufferLayout layout;
	};
}