// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/LightmapDescriptor.hpp"
#include "AssetBuilder/API/Material/MaterialReference.hpp"
#include "AssetBuilder/API/TextureFilteringDescriptor.hpp"

namespace Trident {
	struct AABB2D {
		vec2<float32> min;
		vec2<float32> max;
	};

	struct FaceDescriptor {
		view<vec3<float32>> vertices;
		vec3<float32> normal;
		MaterialReference material;
		TextureFilteringDescriptor textureFiltering;
		LightmapDescriptor lightmap;
	};

	struct IndexedFaceDescriptor {
		view<uint32> indices;
		vec3<float32> normal;
		MaterialReference material;
		TextureFilteringDescriptor textureFiltering;
		LightmapDescriptor lightmap;
	};

	/// The following properties can be updated after a face has been created:
	/// - Vertex positions (might affect other faces)
	/// - Vertex indices
	/// - Material index
	/// - UV translation
	/// - UV rotation
	/// - UV scaling
	/// - Lightmap luxel scale
	/// - Lightmap albedo
	/// - Lightmap emissive color
	/// - Lightmap emissive intensity
	struct Face {
		uint32 getUniqueIndexCount() const {
			return static_cast<uint32>(uniqueIndices.size());
		}

		uint32 getIndexCount() const {
			return static_cast<uint32>(indices.size());
		}

		uint32 getTriangleCount() const {
			DEBUG_ASSERT(indices.size() > 0);

			return static_cast<uint32>(indices.size() - 2);
		}

		// TODO(asset-builder/map/face): Try to remove.
		std::vector<uint32> uniqueIndices;

		/// Vertex indices in clockwise order.
		std::vector<uint32> indices;

		/// Compiled offsets.
		uint32 vertexOffset;
		uint32 triangleOffset;

		vec3<float32> normal;
		uint32 geometryIndex;
		MaterialReference material;
		TextureFilteringDescriptor textureFiltering;
		LightmapDescriptor lightmap;
		AABB2D boundingBox;
	};
}