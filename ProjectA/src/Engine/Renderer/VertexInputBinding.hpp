#pragma once

#include "Engine/Renderer/Buffer/VertexInputAttributeType.hpp"
#include "Engine/Renderer/InputRate.hpp"
#include "Engine/Renderer/VertexInputAttribute.hpp"

namespace ProjectA {
	class VertexInputBinding {
		public:
			VertexInputBinding(InputRate inputRate);

			InputRate getInputRate() const;

			const chain<VertexInputAttribute>& getAttributes() const;

			uint32 getStride() const;

			template<typename AttributeType>
			constexpr void registerAttribute(bool normalized) {
				VertexInputAttribute vertexInputAttribute{};
				vertexInputAttribute.type = getVertexInputAttributeType<AttributeType>();
				vertexInputAttribute.normalized = normalized;
				vertexInputAttribute.offset = stride;

				attributes.emplace_back(vertexInputAttribute);

				const uint32 attributeSize = static_cast<uint32>(sizeof(AttributeType));

				// TODO: uint32 lyah::max(uint32, uint32).
				alignment = std::max(alignment, attributeSize);

				// TODO: Calculate correct required alignment.
				stride += alignment;
			}

			template<>
			constexpr void registerAttribute<mat3x3<float32>>(bool normalized) {
				// Pad 3x3 matrices.
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
			}

			template<>
			constexpr void registerAttribute<mat3x3<float64>>(bool normalized) {
				// Pad 3x3 matrices.
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
			}

			template<>
			constexpr void registerAttribute<mat4x4<float32>>(bool normalized) {
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
			}

			template<>
			constexpr void registerAttribute<mat4x4<float64>>(bool normalized) {
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
			}

		private:
			InputRate inputRate;
			chain<VertexInputAttribute> attributes;
			uint32 alignment = 0;
			uint32 stride = 0;
	};
}