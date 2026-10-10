// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/InputRate.hpp"
#include "Engine/API/Graphics/VertexInputAttribute.hpp"

namespace Trident {
	class VertexInputBinding {
		public:
			explicit VertexInputBinding(
				/// The binding's input rate.
				InputRate inputRate,

				/// If true, attributes are automatically aligned.
				/// If false, no padding is applied.
				bool pad = true
			);

			/// Returns the binding's input rate.
			InputRate getInputRate() const;

			/// Returns the vertex input attributes.
			view<VertexInputAttribute> getAttributes() const;

			/// Returns the alignment of the largest attribute.
			uint32 getMaxAlignment() const;

			template<typename AttributeType>
			constexpr void registerAttribute(VertexInputAttributeFormat format) {
				VertexInputAttribute attribute{};
				attribute.format = format;
				attribute.offset = offset;

				attributes.emplace_back(attribute);

				constexpr uint32 size = sizeof(AttributeType);

				offset += size;
				stride = offset;
				maxAlignment = max(maxAlignment, size);
			}

			template<typename AttributeType>
			constexpr void registerAttribute() {
				const VertexInputAttributeFormat format = getVertexInputAttributeFormat<AttributeType>();

				return registerAttribute<AttributeType>(format);
			}

		public:
			/// The stride may be changed manually if it goes beyond the provided attributes.
			/// Note that any call to `registerAttribute` will increment it.
			uint32 stride = 0;

		private:
			InputRate inputRate = InputRate::VERTEX;
			std::vector<VertexInputAttribute> attributes;
			uint32 offset = 0;
			uint32 maxAlignment = 0;
			bool pad;
	};

	template<>
	void VertexInputBinding::registerAttribute<mat2x2<float32>>();

	template<>
	void VertexInputBinding::registerAttribute<mat3x3<float32>>();

	template<>
	void VertexInputBinding::registerAttribute<mat4x4<float32>>();
}