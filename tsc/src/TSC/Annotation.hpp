#pragma once

namespace ProjectA {
	enum class Annotation : uint8 {
		BINDING = BIT(0),
		CONSTANT = BIT(1),
		INSTANCE_INDEX = BIT(2),
		LOCATION = BIT(3),
		POSITION = BIT(4),
		SET = BIT(5),
		VERTEX_INDEX = BIT(6),
	};

	AS_FLAG(Annotation);

	static constexpr uint8 ANNOTATION_COUNT = 7;

	enum class AnnotationType : uint8 {
		NONE,
		UINT32,
	};

	static constexpr Annotation ANNOTATION_TYPE_VOID = Annotation::INSTANCE_INDEX | Annotation::POSITION | Annotation::VERTEX_INDEX;
	static constexpr Annotation ANNOTATION_TYPE_UINT32 = Annotation::BINDING | Annotation::CONSTANT | Annotation::LOCATION | Annotation::SET;

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