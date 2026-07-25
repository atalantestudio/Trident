// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/InputRate.hpp"
#include "Engine/API/Graphics/VertexInputAttributeType.hpp"
#include "Engine/API/Graphics/VertexInputAttribute.hpp"

namespace Trident {
	class VertexInputBinding {
		public:
			// Needed for copying a sequence of bindings.
			VertexInputBinding() = default;

			explicit VertexInputBinding(InputRate inputRate, bool pad = true) :
				inputRate(inputRate),
				pad(pad)
			{}

			inline InputRate getInputRate() const {
				return inputRate;
			}

			inline const std::vector<VertexInputAttribute>& getAttributes() const {
				return attributes;
			}

			inline uint32 getMaxAlignment() const {
				return maxAlignment;
			}

			template<typename AttributeType>
			constexpr void registerAttribute() {
				constexpr uint32 alignment = getAlignment<AttributeType>();

				uint32 remainder = offset % alignment;

				if (remainder != 0 && pad) {
					// Pad so that the offset is a multiple of the alignment.
					offset += alignment - remainder;
				}

				VertexInputAttribute vertexInputAttribute{};
				vertexInputAttribute.type = getVertexInputAttributeType<AttributeType>();
				vertexInputAttribute.offset = offset;

				attributes.emplace_back(vertexInputAttribute);

				const uint32 size = static_cast<uint32>(sizeof(AttributeType));

				offset += size;
				stride = offset;
				maxAlignment = max(maxAlignment, alignment);
				remainder = stride % maxAlignment;

				if (remainder != 0 && pad) {
					// Pad so that the stride is a multiple of the alignment.
					stride += maxAlignment - remainder;
				}
			}

		public:
			// NOTE
			// The stride may be changed manually if it goes beyond the provided attributes.
			// Keep in mind that calls to VertexInputBinding::registerAttribute will add to it.
			uint32 stride = 0;

		private:
			InputRate inputRate = InputRate::VERTEX;
			std::vector<VertexInputAttribute> attributes;
			uint32 offset = 0;
			uint32 maxAlignment = 0;

			/// If true, attributes are automatically aligned.
			/// If false, no padding is applied.
			bool pad;
	};

	template<>
	inline constexpr void VertexInputBinding::registerAttribute<mat3x3<float32>>() {
		registerAttribute<vec3<float32>>();
		registerAttribute<vec3<float32>>();
		registerAttribute<vec3<float32>>();
	}

	template<>
	inline constexpr void VertexInputBinding::registerAttribute<mat3x3<float64>>() {
		registerAttribute<vec3<float64>>();
		registerAttribute<vec3<float64>>();
		registerAttribute<vec3<float64>>();
	}

	template<>
	inline constexpr void VertexInputBinding::registerAttribute<mat4x4<float32>>() {
		registerAttribute<vec4<float32>>();
		registerAttribute<vec4<float32>>();
		registerAttribute<vec4<float32>>();
		registerAttribute<vec4<float32>>();
	}

	template<>
	inline constexpr void VertexInputBinding::registerAttribute<mat4x4<float64>>() {
		registerAttribute<vec4<float64>>();
		registerAttribute<vec4<float64>>();
		registerAttribute<vec4<float64>>();
		registerAttribute<vec4<float64>>();
	}
}