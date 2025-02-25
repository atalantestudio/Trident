#pragma once

namespace ProjectA {
	class IndexBuffer {
		protected:
			uint32 id;

		public:
			static ref<IndexBuffer> create(const uint32* indices, uint32 count);

			virtual ~IndexBuffer() = default;

			virtual uint32 getCount() const = 0;

			virtual void bind() const = 0;

			virtual void unbind() const = 0;
	};
}