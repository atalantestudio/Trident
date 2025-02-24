#include "pch.hpp"

#include "Engine/Renderer/BufferAttribute.hpp"

namespace ProjectA {
	BufferAttribute::BufferAttribute(const uint32 glType, const uint32 size, const uint32 componentCount, const std::size_t offset, const bool normalized) :
		glType(glType),
		size(size),
		componentCount(componentCount),
		offset(offset),
		normalized(normalized)
	{}
}