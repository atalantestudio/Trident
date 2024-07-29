#pragma once

#include "BufferElement.hpp"

namespace ProjectA {
	class BufferLayout {
		private:
			std::vector<BufferElement> elements;
			uint32_t stride = 0;

			void calculateOffsetsAndStride();

		public:
			BufferLayout();

			BufferLayout(const std::initializer_list<BufferElement>& elements);

			inline const std::vector<BufferElement>& getElements() const {
				return elements;
			}

			inline uint32_t getStride() const {
				return stride;
			}

			std::vector<BufferElement>::iterator begin();

			std::vector<BufferElement>::iterator end();

			std::vector<BufferElement>::const_iterator begin() const;

			std::vector<BufferElement>::const_iterator end() const;
	};
}