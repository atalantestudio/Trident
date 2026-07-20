// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/StaticGeometry3D.hpp"

namespace Trident {
	struct TextureFilteringDescriptor {
		svec2<float32> translation;
		float32 rotation;
		svec2<float32> scale;
	};

	struct Carving {
		/// Offset from the top left.
		svec2<float32> offset;

		svec2<float32> size;
	};

	struct SubsurfaceQuad {
		svec2<float32> start;
		svec2<float32> end;
	};

	struct SurfaceDescriptor {
		StaticGeometry3D geometry;
		TextureFilteringDescriptor textureFiltering;
		uint32 materialIndex;
		svec2<float32> luxelScale;
		std::vector<Carving> carvings;
	};

	struct DualSnapSurfaceDescriptor {
		TextureFilteringDescriptor textureFiltering;
		uint32 materialIndex;
		svec2<float32> luxelScale;
		std::vector<Carving> carvings;
	};

	struct Transform {
		/// Relative to the surface center.
		vec3<float32> translation;

		/// Rotation axis.
		svec3<float32> axis;

		/// Rotation angle in radians.
		float32 angle;
	};

	struct StaticEdge {
		/// Vertices in world space.
		vec3<float32> a;
		vec3<float32> b;

		// TODO(asset-builder/scene/edge): Remove unused normal?
		/// Outwards direction in regard to the parent surface.
		vec3<float32> normal;
	};

	struct StaticSubsurface {
		inline StaticEdge getEdge(view<uint16> edgeOriginVertexIndices, uint32 edgeIndex) const {
			const uint16 offsetA = edgeOriginVertexIndices[edgeIndex] * 3;
			const uint16 offsetB = edgeOriginVertexIndices[(edgeIndex + 1) % edgeOriginVertexIndices.count()] * 3;

			return {
				.a = {vertexComponents[offsetA + 0], vertexComponents[offsetA + 1], vertexComponents[offsetA + 2]},
				.b = {vertexComponents[offsetB + 0], vertexComponents[offsetB + 1], vertexComponents[offsetB + 2]},

				// TODO(asset-builder/scene/edge): Remove unused normal?
			};
		}

		sequence<float32> vertexComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexIndices;
		sequence<uint32> vertexTextureCoordinateIndices;
		svec2<float32> scale;
	};

	struct StaticSurface {
		inline StaticEdge getEdge(uint32 edgeIndex) const {
			const uint16 indexA = edgeOriginVertexIndices[edgeIndex];
			const uint16 indexB = edgeOriginVertexIndices[(edgeIndex + 1) % edgeOriginVertexIndices.count()];
			const vec3<float32> a = vertices[indexA];
			const vec3<float32> b = vertices[indexB];

			return {
				.a = a,
				.b = b,
				.normal = normalized(cross(b - a, normal)),
			};
		}

		/// Vertices in world space, used for snapping.
		sequence<svec3<float32>> vertices;

		sequence<uint16> edgeOriginVertexIndices;
		svec3<float32> normal;
		svec2<float32> scale;
		std::vector<StaticSubsurface> subsurfaces;
		std::vector<StaticSubsurface> hollowSubsurfaces;
		TextureFilteringDescriptor textureFiltering;
		uint32 triangleOffset;
		uint32 materialIndex;
		svec2<float32> luxelScale;
	};
}