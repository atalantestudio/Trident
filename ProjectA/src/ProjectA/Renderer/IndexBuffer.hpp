#pragma once

namespace ProjectA {
	class IndexBuffer {
		protected:
			uint32_t id;

		public:
			static IndexBuffer* create(uint32_t* indices, uint32_t count);

			virtual ~IndexBuffer() {}

			virtual uint32_t getCount() const = 0;

			virtual void bind() const = 0;

			virtual void unbind() const = 0;
	};
}