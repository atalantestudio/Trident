#pragma once

#include "ShaderType.hpp"

namespace ProjectA {
	struct BufferElement {
		ShaderType type;
		std::string name;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement(ShaderType type, const std::string& name, bool normalized);
	};
}