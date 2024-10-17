#pragma once

#include "BufferLayout.hpp"

namespace ProjectA {
	class VertexBuffer {
		protected:
			uint32_t id;

		public:
			static Ref<VertexBuffer> create(float* vertices, uint32_t size);

			virtual ~VertexBuffer() = default;

			virtual const BufferLayout& getLayout() const = 0;

			virtual void setLayout(const BufferLayout&) = 0;

			virtual void bind() const = 0;

			virtual void unbind() const = 0;
	};
}