// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "FBX.hpp"

namespace Trident::FBX {
	const Node* findNode(const File<FileFormat::FBX>& file, view<char8> name) {
		for (const uint64 rootNodeIndex : file.rootNodeIndices) {
			const Node* node = &file.nodes[rootNodeIndex];

			if (node->name == name) {
				return node;
			}
		}

		return nullptr;
	}

	const Node* findNode(const File<FileFormat::FBX>& file, const Node* node, view<char8> name) {
		const Node* searchNode = nullptr;
		uint64 nodeIndex = 0;

		while (nodeIndex < node->nodeRange.count) {
			const Node* childNode = &file.nodes[node->nodeRange.offset + nodeIndex];

			if (childNode->name == name) {
				searchNode = childNode;

				break;
			}

			searchNode = findNode(file, childNode, name);

			if (searchNode != nullptr) {
				break;
			}

			nodeIndex += 1;
		}

		return searchNode;
	}
}