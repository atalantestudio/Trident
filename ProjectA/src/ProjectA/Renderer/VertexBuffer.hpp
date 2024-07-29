#pragma once

namespace ProjectA {
	class VertexBuffer {
		protected:
			uint32_t id;

		public:
			static VertexBuffer* create(float* vertices, uint32_t size);

			virtual ~VertexBuffer() = default;

			virtual void bind() const = 0;

			virtual void unbind() const = 0;
	};
}