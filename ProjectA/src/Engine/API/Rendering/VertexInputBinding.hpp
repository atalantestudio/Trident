#pragma once

#include "Engine/API/Rendering/InputRate.hpp"
#include "Engine/API/Rendering/VertexInputAttributeType.hpp"
#include "Engine/API/Rendering/VertexInputAttribute.hpp"

namespace ProjectA {
	class VertexInputBinding {
		public:
			// Needed for copying a sequence of bindings.
			VertexInputBinding() = default;

			explicit VertexInputBinding(InputRate inputRate) :
				inputRate(inputRate)
			{}

			InputRate getInputRate() const {
				return inputRate;
			}

			const std::vector<VertexInputAttribute>& getAttributes() const {
				return attributes;
			}

			uint32 getStride() const {
				return stride;
			}

			uint32 getMaxAlignment() const {
				return maxAlignment;
			}

			template<typename AttributeType>
			constexpr void registerAttribute() {
				constexpr uint32 alignment = getAlignment<AttributeType>();

				uint32 remainder = offset % alignment;

				if (remainder != 0) {
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

				if (remainder != 0) {
					// Pad so that the stride is a multiple of the alignment.
					stride += maxAlignment - remainder;
				}
			}

			template<>
			constexpr void registerAttribute<mat3x3<float32>>() {
				registerAttribute<vec3<float32>>();
				registerAttribute<vec3<float32>>();
				registerAttribute<vec3<float32>>();
			}

			template<>
			constexpr void registerAttribute<mat3x3<float64>>() {
				registerAttribute<vec3<float64>>();
				registerAttribute<vec3<float64>>();
				registerAttribute<vec3<float64>>();
			}

			template<>
			constexpr void registerAttribute<mat4x4<float32>>() {
				registerAttribute<vec4<float32>>();
				registerAttribute<vec4<float32>>();
				registerAttribute<vec4<float32>>();
				registerAttribute<vec4<float32>>();
			}

			template<>
			constexpr void registerAttribute<mat4x4<float64>>() {
				registerAttribute<vec4<float64>>();
				registerAttribute<vec4<float64>>();
				registerAttribute<vec4<float64>>();
				registerAttribute<vec4<float64>>();
			}

		private:
			InputRate inputRate = InputRate::VERTEX;
			std::vector<VertexInputAttribute> attributes;
			uint32 offset = 0;
			uint32 stride = 0;
			uint32 maxAlignment = 0;
	};
}