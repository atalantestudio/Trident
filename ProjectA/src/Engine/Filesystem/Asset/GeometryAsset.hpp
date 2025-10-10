#pragma once

#include "Engine/API/Geometry/Index.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace ProjectA {
	struct GeometryAssetDescriptor : public AssetDescriptor {
		// Per-instance cluster count.
		uint32 clusterCount;

		// Position components (X/Y/Z).
		sequence<float32> vertexPositionComponents;

		// Indices arranged in clusters. Not including degenerate indices.
		sequence<Index> indices;
	};

	class GeometryAsset : public Asset {
		public:
			explicit GeometryAsset(const GeometryAssetDescriptor& descriptor) :
				Asset(descriptor),
				clusterCount(descriptor.clusterCount),
				vertexPositionComponents(descriptor.vertexPositionComponents),
				indices(descriptor.indices)
			{}

			uint32 getClusterCount() const {
				return clusterCount;
			}

			const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			const sequence<Index>& getIndices() const {
				return indices;
			}

		private:
			uint32 clusterCount;
			sequence<float32> vertexPositionComponents;
			sequence<Index> indices;
	};
}