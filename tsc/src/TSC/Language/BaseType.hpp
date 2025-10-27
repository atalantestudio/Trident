#pragma once

namespace ProjectA {
	enum BaseType : uint8 {
		// TODO
		OTHER,

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

	static constexpr uint32 TYPE_TEMPLATE_ARGUMENT_MASKS[] = {
		0,
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

	static constexpr uint8 TYPE_COMPONENT_COUNTS[] = {
		0,
		1,
		1,
		1,
		1,
		2,
		3,
		4,
		4,
		8,
		8,
		6,
		12,
		12,
		8,
		16,
		16,
	};

	static constexpr uint8 TYPE_ALIGNMENTS[] = {
		0,
		4,
		8,
		4,
		4,

		// Multiply by the scalar alignment.
		2,
		4,
		4,

		// Multiply by the scalar alignment.
		2,
		4,
		4,
		2,
		4,
		4,
		2,
		4,
		4,
	};
}