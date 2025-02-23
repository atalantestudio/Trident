#pragma once

#include "ProjectA/Renderer/BufferLayout.hpp"

namespace ProjectA {
	class VertexBuffer {
		protected:
			uint32 id;

		public:
			static ref<VertexBuffer> create(uint32 size);
			static ref<VertexBuffer> create(float32* vertices, uint32 size);

			virtual ~VertexBuffer() = default;

			virtual const BufferLayout& getLayout() const = 0;

			virtual void setLayout(const BufferLayout&) = 0;

			virtual void copyData(const void* data, uint32 size) = 0;

			virtual void bind() const = 0;

			virtual void unbind() const = 0;
	};
}