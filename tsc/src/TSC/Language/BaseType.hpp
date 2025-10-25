#pragma once

namespace ProjectA {
	enum BaseType : uint8 {
		// Scalar types
		FLOAT32,
		FLOAT64,
		SINT32,
		UINT32,

		// Vector types
		VEC2,
		VEC3,
		VEC4,

		// Matrix types
		MAT2X2,
		MAT2X3,
		MAT2X4,
		MAT3X2,
		MAT3X3,
		MAT3X4,
		MAT4X2,
		MAT4X3,
		MAT4X4,
	};

	// TODO: Remember to change the mask size when adding more types.
	static constexpr uint16 TEMPLATE_ARGUMENT_MASKS[] = {
		0,
		0,
		0,
		0,
		BIT(FLOAT32) | BIT(FLOAT64) | BIT(SINT32) | BIT(UINT32),
		BIT(FLOAT32) | BIT(FLOAT64) | BIT(SINT32) | BIT(UINT32),
		BIT(FLOAT32) | BIT(FLOAT64) | BIT(SINT32) | BIT(UINT32),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
		BIT(FLOAT32) | BIT(FLOAT64),
	};
}