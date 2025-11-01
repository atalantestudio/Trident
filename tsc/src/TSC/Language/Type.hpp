#pragma once

namespace ProjectA {
	enum Type : uint32 {
		NONE,

		FUNCTION = NONE,

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

		// NOTE
		// User-defined types are indexed relatively to this value.
		// E.g. the third user-defined type will be USER_DEFINED + 2.
		USER_DEFINED,
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

	static constexpr uint8 TYPE_ALIGNED_COMPONENT_COUNTS[] = {
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

		// NOTE
		// Below are the aligned component counts for vector and matrix types.
		// To get the alignment, multiply by the template argument (scalar) alignment.
		2,
		4,
		4,
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
		6,
		8,
		6,
		9,
		12,
		8,
		12,
		16,
	};
}