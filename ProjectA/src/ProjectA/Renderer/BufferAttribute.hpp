#pragma once

#include "ProjectA/Renderer/Attribute.hpp"

// TODO: Remove
#include "Platform/OpenGL/Attribute.hpp"

namespace ProjectA {
	class BufferAttribute {
		public:
			uint32 glType;
			uint32 size;
			uint32 componentCount;
			std::size_t offset;
			bool normalized;

		public:
			// NOTE: The attribute name is not needed by the vertex array
			// but is present here for tracking purposes.
			template<typename T>
			static BufferAttribute create(const std::string& name, const bool normalized = false) {
				return {getAttributeGlType<T>(), getAttributeSize<T>(), getAttributeComponentCount<T>(), 0, normalized};
			}

			BufferAttribute(const uint32 glType, const uint32 size, const uint32 componentCount, const std::size_t offset, const bool normalized);
	};
}