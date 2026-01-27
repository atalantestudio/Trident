// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct GeometryAssetDescriptor : public AssetDescriptor {
		// Per-instance cluster count.
		uint32 clusterCount;

		// 3D vertex components.
		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexNormalComponents;
		sequence<float32> vertexTextureCoordinateComponents;

		// Indices arranged in clusters. Not including degenerate indices.
		sequence<uint32> vertexPositionIndices;
		sequence<uint32> vertexNormalIndices;
		sequence<uint32> vertexTextureCoordinateIndices;
	};

	class GeometryAsset : public Asset {
		public:
			explicit GeometryAsset(const GeometryAssetDescriptor& descriptor) :
				Asset(descriptor),
				clusterCount(descriptor.clusterCount),
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexNormalComponents(descriptor.vertexNormalComponents),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				vertexPositionIndices(descriptor.vertexPositionIndices),
				vertexNormalIndices(descriptor.vertexNormalIndices),
				vertexTextureCoordinateIndices(descriptor.vertexTextureCoordinateIndices)
			{}

			uint32 getClusterCount() const {
				return clusterCount;
			}

			const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			const sequence<float32>& getVertexNormalComponents() const {
				return vertexNormalComponents;
			}

			const sequence<float32>& getVertexTextureCoordinateComponents() const {
				return vertexTextureCoordinateComponents;
			}

			const sequence<uint32>& getVertexPositionIndices() const {
				return vertexPositionIndices;
			}

			const sequence<uint32>& getVertexNormalIndices() const {
				return vertexNormalIndices;
			}

			const sequence<uint32>& getVertexTextureCoordinateIndices() const {
				return vertexTextureCoordinateIndices;
			}

		private:
			uint32 clusterCount;
			sequence<float32> vertexPositionComponents;
			sequence<float32> vertexNormalComponents;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<uint32> vertexNormalIndices;
			sequence<uint32> vertexTextureCoordinateIndices;
	};
}