#pragma once

#include "ShaderType.hpp"

namespace ProjectA {
	struct BufferElement {
		ShaderType type;
		std::string name;
		std::uint32_t size;
		std::size_t offset;
		bool normalized;

		BufferElement() = default;
		BufferElement(ShaderType type, const std::string& name, bool normalized);
	};
}