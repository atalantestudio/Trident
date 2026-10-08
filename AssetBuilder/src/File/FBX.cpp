// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "FBX.hpp"

namespace Trident::FBX {
	const Node* findNode(const File<FileFormat::FBX>& file, view<char8> name) {
		uint64 rootNodeIndex = 0;

		while (rootNodeIndex < file.rootNodeCount) {
			const Node* rootNode = &file.nodes[rootNodeIndex];

			if (rootNode->name == name) {
				return rootNode;
			}

			rootNodeIndex += 1;
		}

		DEBUG_ASSERT(false);

		return nullptr;
	}

	const Node* findNode(const File<FileFormat::FBX>& file, const Node* node, view<char8> name) {
		uint64 nodeIndex = 0;

		while (nodeIndex < node->nodeRange.count) {
			const Node* childNode = &file.nodes[node->nodeRange.offset + nodeIndex];

			if (childNode->name == name) {
				return childNode;
			}

			nodeIndex += 1;
		}

		DEBUG_ASSERT(false);

		return nullptr;
	}

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

	#if defined(BUILD_MODE_DEBUG)
		void FBX::debugFile(const File<FileFormat::FBX>& file, ConsoleLogger& logger) {
			ENGINE_DEBUG("Debugging FBX [] file.", file.version);

			uint64 rootNodeIndex = 0;

			while (rootNodeIndex < file.rootNodeCount) {
				debugNode(file, &file.nodes[rootNodeIndex], logger, 0);

				rootNodeIndex += 1;
			}

			ENGINE_DEBUG("Debugging stopped.");
		}

		void FBX::debugNode(const File<FileFormat::FBX>& file, const Node* node, ConsoleLogger& logger, uint8 indentSize) {
			const std::string indent(indentSize * 4, ' ');

			std::ostream& stream = logger.getOutputStream();
			stream << indent << '"' << node->name << '"' << '\n';

			if (node->propertyRange.count > 0) {
				stream << indent << "  Properties:\n";

				uint64 propertyIndex = 0;

				while (propertyIndex < node->propertyRange.count) {
					debugProperty(file.properties[node->propertyRange.offset + propertyIndex], logger, indentSize + 1);

					propertyIndex += 1;
				}
			}

			if (node->nodeRange.count > 0) {
				stream << indent << "  Nodes:" << '\n';

				uint64 nodeIndex = 0;

				while (nodeIndex < node->nodeRange.count) {
					debugNode(file, &file.nodes[node->nodeRange.offset + nodeIndex], logger, indentSize + 1);

					nodeIndex += 1;
				}
			}
		}

		void FBX::debugProperty(ref<Property> property, ConsoleLogger& logger, uint8 indentSize) {
			const std::string indent(indentSize * 4, ' ');

			std::ostream& stream = logger.getOutputStream();
			stream << indent << '[' << static_cast<char8>(property->typeCode) << ']';

			switch (property->typeCode) {
				case PropertyTypeCode::Y: {
					stream << ' ' << ref_cast<PrimitiveProperty<sint16>>(property)->data << '\n';

					break;
				}
				case PropertyTypeCode::C: {
					stream << ' ' << ref_cast<PrimitiveProperty<bool>>(property)->data << '\n';

					break;
				}
				case PropertyTypeCode::I: {
					stream << ' ' << ref_cast<PrimitiveProperty<sint32>>(property)->data << '\n';

					break;
				}
				case PropertyTypeCode::F: {
					stream << ' ' << ref_cast<PrimitiveProperty<float32>>(property)->data << '\n';

					break;
				}
				case PropertyTypeCode::D: {
					stream << ' ' << ref_cast<PrimitiveProperty<float64>>(property)->data << '\n';

					break;
				}
				case PropertyTypeCode::L: {
					stream << ' ' << ref_cast<PrimitiveProperty<sint64>>(property)->data << '\n';

					break;
				}
				case PropertyTypeCode::S: {
					const ref<SpecialProperty> specialProperty = ref_cast<SpecialProperty>(property);
					const view<char8> data(pointer_cast<const char8*>(&specialProperty->data[0]), specialProperty->data.count());

					stream << " \"" << data << "\"\n";

					break;
				}
				case PropertyTypeCode::R: {
					const ref<SpecialProperty> specialProperty = ref_cast<SpecialProperty>(property);

					stream << '[' << specialProperty->data.count() << "]\n";

					break;
				}
				default: {
					// We only care about the count here, not the type.
					const ref<ArrayProperty<bool>> arrayProperty = ref_cast<ArrayProperty<bool>>(property);

					stream << '[' << arrayProperty->contents.count() << "]\n";

					break;
				}
			}
		}
	#endif
}