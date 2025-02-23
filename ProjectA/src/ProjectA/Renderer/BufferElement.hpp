#pragma once

#include "ShaderType.hpp"

namespace ProjectA {
	class BufferElement {
		public:
			ShaderType type;
			std::string name;
			std::uint32_t size;
			std::size_t offset;
			bool normalized;

		public:
			BufferElement() = default;
			BufferElement(const ShaderType type, const std::string& name, const bool normalized);
	};
}