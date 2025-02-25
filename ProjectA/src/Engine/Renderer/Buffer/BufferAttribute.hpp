#pragma once

// TODO: Remove
#include "Platform/OpenGL/Attribute.hpp"

#include "Engine/Renderer/Buffer/Attribute.hpp"

namespace ProjectA {
	class BufferAttribute {
		public:
			uint32 glType;
			uint32 size;
			uint32 componentCount;
			std::size_t offset;
			bool normalized;

		public:
			template<typename T>
			static BufferAttribute create(const bool normalized = false) {
				return {getAttributeGlType<T>(), getAttributeSize<T>(), getAttributeComponentCount<T>(), 0, normalized};
			}

			BufferAttribute(const uint32 glType, const uint32 size, const uint32 componentCount, const std::size_t offset, const bool normalized);
	};
}