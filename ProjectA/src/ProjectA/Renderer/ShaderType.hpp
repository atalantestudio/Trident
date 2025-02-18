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
	static uint32 getShaderTypeSize(ShaderType type) {
		switch (type) {
			case ShaderType::Bool:
				return sizeof(bool);
			case ShaderType::Int:
				return sizeof(int32);
			case ShaderType::Int2:
				return sizeof(int32) * 2;
			case ShaderType::Int3:
				return sizeof(int32) * 3;
			case ShaderType::Int4:
				return sizeof(int32) * 4;
			case ShaderType::Float:
				return sizeof(float32);
			case ShaderType::Float2:
				return sizeof(float32) * 2;
			case ShaderType::Float3:
				return sizeof(float32) * 3;
			case ShaderType::Float4:
				return sizeof(float32) * 4;
			case ShaderType::Mat3:
				return sizeof(float32) * 3 * 3;
			case ShaderType::Mat4:
				return sizeof(float32) * 4 * 4;
		}

		ENGINE_ASSERT(false, "Unhandled shader type.");

		return 0;
	}

	// TODO: Replace with typename template
	// Use primitive types and math library types
	static uint32 getShaderTypeComponentCount(ShaderType type) {
		switch (type) {
			case ShaderType::Bool:
			case ShaderType::Int:
			case ShaderType::Float:
				return 1;
			case ShaderType::Int2:
			case ShaderType::Float2:
				return 2;
			case ShaderType::Int3:
			case ShaderType::Float3:
				return 3;
			case ShaderType::Int4:
			case ShaderType::Float4:
				return 4;
			case ShaderType::Mat3:
				return 3 * 3;
			case ShaderType::Mat4:
				return 4 * 4;
		}

		ENGINE_ASSERT(false, "Unhandled shader type.");

		return 0;
	}
}