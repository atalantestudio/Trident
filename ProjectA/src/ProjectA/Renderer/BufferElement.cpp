#include "pch.hpp"
#include "BufferElement.hpp"

namespace ProjectA {
	BufferElement::BufferElement(ShaderType type, const std::string& name, bool normalized) : type(type), name(name), size(getShaderTypeSize(type)), offset(0), normalized(normalized) {}
}