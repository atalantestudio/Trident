// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	namespace FBX {
		template<typename DataType>
		DataType resolvePrimitiveProperty(const File<FileFormat::FBX>& file, const Node* parentNode, view<char8> name) {
			uint64 nodeIndex = 0;

			while (nodeIndex < parentNode->nodeRange.count) {
				const Node* node = &file.nodes[parentNode->nodeRange.offset + nodeIndex];

				if (node->propertyRange.count < 5) {
					nodeIndex += 1;

					continue;
				}

				const ref<Property> firstProperty = file.properties[node->propertyRange.offset];

				if (firstProperty->typeCode != PropertyTypeCode::S) {
					nodeIndex += 1;

					continue;
				}

				const ref<SpecialProperty> nameProperty = ref_cast<SpecialProperty>(firstProperty);
				const view<char8> propertyName(pointer_cast<const char8*>(&nameProperty->data[0]), nameProperty->data.count());

				if (propertyName != name) {
					nodeIndex += 1;

					continue;
				}

				const ref<PrimitiveProperty<DataType>> valueProperty = ref_cast<PrimitiveProperty<DataType>>(file.properties[node->propertyRange.offset + 4]);

				return valueProperty->data;
			}

			DEBUG_ASSERT(false);

			return static_cast<DataType>(0);
		}

		template<typename DataType>
		vec3<DataType> resolveVec3Property(const File<FileFormat::FBX>& file, const Node* parentNode, view<char8> name, vec3<DataType> defaultValue) {
			uint64 nodeIndex = 0;

			while (nodeIndex < parentNode->nodeRange.count) {
				const Node* node = &file.nodes[parentNode->nodeRange.offset + nodeIndex];

				if (node->propertyRange.count < 7) {
					nodeIndex += 1;

					continue;
				}

				const ref<Property> firstProperty = file.properties[node->propertyRange.offset];

				if (firstProperty->typeCode != PropertyTypeCode::S) {
					nodeIndex += 1;

					continue;
				}

				const ref<SpecialProperty> nameProperty = ref_cast<SpecialProperty>(firstProperty);
				const view<char8> propertyName(pointer_cast<const char8*>(&nameProperty->data[0]), nameProperty->data.count());

				if (propertyName != name) {
					nodeIndex += 1;

					continue;
				}

				const ref<PrimitiveProperty<DataType>> xProperty = ref_cast<PrimitiveProperty<DataType>>(file.properties[node->propertyRange.offset + 4]);
				const ref<PrimitiveProperty<DataType>> yProperty = ref_cast<PrimitiveProperty<DataType>>(file.properties[node->propertyRange.offset + 5]);
				const ref<PrimitiveProperty<DataType>> zProperty = ref_cast<PrimitiveProperty<DataType>>(file.properties[node->propertyRange.offset + 6]);

				return {xProperty->data, yProperty->data, zProperty->data};
			}

			return defaultValue;
		}
	}
}