#pragma once

namespace ProjectA {
	enum Annotation : uint8 {
		BINDING,
		CONSTANT,
		INSTANCE_INDEX,
		LOCATION,
		POSITION,
		SET,
		VERTEX_INDEX,

		ANNOTATION_COUNT,
	};

	static constexpr uint32 ENTRY_POINT_ANNOTATION_MASK = BIT(INSTANCE_INDEX) | BIT(POSITION) | BIT(VERTEX_INDEX);
	static constexpr uint32 ANNOTATION_TYPE_NONE = BIT(INSTANCE_INDEX) | BIT(POSITION) | BIT(VERTEX_INDEX);
	static constexpr uint32 ANNOTATION_TYPE_UINT32 = BIT(BINDING) | BIT(CONSTANT) | BIT(LOCATION) | BIT(SET);

	static constexpr const char8* ANNOTATION_STRINGS[ANNOTATION_COUNT] = {
		"binding",
		"constant",
		"instanceIndex",
		"location",
		"position",
		"set",
		"vertexIndex",
	};
}