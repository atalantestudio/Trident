#pragma once

#include "Engine/Renderer/Buffer/BufferAttribute.hpp"

namespace ProjectA {
	class BufferLayout {
		public:
			std::vector<BufferAttribute> attributes;
			uint32 stride = 0;

		public:
			BufferLayout() = default;
			BufferLayout(const std::initializer_list<BufferAttribute>& attributes);

			std::vector<BufferAttribute>::iterator begin();
			std::vector<BufferAttribute>::const_iterator begin() const;

			std::vector<BufferAttribute>::iterator end();
			std::vector<BufferAttribute>::const_iterator end() const;

		private:
			void updateStride();
	};
}