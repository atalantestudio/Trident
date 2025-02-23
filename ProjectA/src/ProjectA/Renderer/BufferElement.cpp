#include "pch.hpp"

#include "ProjectA/Renderer/BufferElement.hpp"

namespace ProjectA {
	BufferElement::BufferElement(const ShaderType type, const std::string& name, const bool normalized) :
		type(type),
		name(name),
		size(getShaderTypeSize(type)),
		offset(0),
		normalized(normalized)
	{}
}