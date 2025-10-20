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
		// VOID,
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

	static std::string getAnnotationString(Annotation annotation) {
		switch (annotation) {
			case Annotation::BINDING: return "binding";
			case Annotation::CONSTANT: return "constant";
			case Annotation::INSTANCE_INDEX: return "instanceIndex";
			case Annotation::LOCATION: return "location";
			case Annotation::POSITION: return "position";
			case Annotation::SET: return "set";
			case Annotation::VERTEX_INDEX: return "vertexIndex";
			default: return "[unknown annotation]";
		}
	}
}