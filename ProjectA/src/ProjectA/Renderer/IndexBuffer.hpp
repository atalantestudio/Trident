#pragma once

namespace ProjectA {
	class IndexBuffer {
		protected:
			uint32_t id;

		public:
			static ref<IndexBuffer> create(const uint32_t* indices, uint32_t count);

			virtual ~IndexBuffer() = default;

			virtual uint32_t getCount() const = 0;

			virtual void bind() const = 0;

			virtual void unbind() const = 0;
	};
}