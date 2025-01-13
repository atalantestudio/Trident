#pragma once

namespace ProjectA {
	// TODO: Replace with typename template
	// Use primitive types and math library types
	enum ShaderType {
		None = 0,
		Bool,
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
	};

	// TODO: Replace with typename template
	// Use primitive types and math library types
	static uint32_t getShaderTypeSize(ShaderType type) {
		switch (type) {
			case ShaderType::Bool:
				return 1;
			case ShaderType::Int:
				return 4;
			case ShaderType::Int2:
				return 4 * 2;
			case ShaderType::Int3:
				return 4 * 3;
			case ShaderType::Int4:
				return 4 * 4;
			case ShaderType::Float:
				return 4;
			case ShaderType::Float2:
				return 4 * 2;
			case ShaderType::Float3:
				return 4 * 3;
			case ShaderType::Float4:
				return 4 * 4;
			case ShaderType::Mat3:
				return 4 * 3 * 3;
			case ShaderType::Mat4:
				return 4 * 4 * 4;
		}

		ENGINE_ASSERT(false, "Unknown ShaderType");

		return 0;
	}

	static uint32_t getShaderTypeComponentCount(ShaderType type) {
		switch (type) {
			case ShaderType::Bool:
				return 1;
			case ShaderType::Int:
				return 1;
			case ShaderType::Int2:
				return 2;
			case ShaderType::Int3:
				return 3;
			case ShaderType::Int4:
				return 4;
			case ShaderType::Float:
				return 1;
			case ShaderType::Float2:
				return 2;
			case ShaderType::Float3:
				return 3;
			case ShaderType::Float4:
				return 4;
			case ShaderType::Mat3:
				return 3 * 3;
			case ShaderType::Mat4:
				return 4 * 4;
		}

		ENGINE_ASSERT(false, "Unknown ShaderType");

		return 0;
	}
}