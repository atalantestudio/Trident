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
	};

	struct DualSnapEdge {
		/// In world space.
		vec3<float32> vertexA;
		vec3<float32> vertexB;

		/// In texture space.
		vec2<float32> uvA;
		vec2<float32> uvB;
	};

	struct StaticSubsurface {
		inline uint32 getTriangleCount() const {
			return static_cast<uint32>(vertexIndices.count() / 3);
		}

		inline StaticEdge getEdge(uint32 edgeIndex) const {
			static constexpr uint16 edgeOriginVertexIndexCount = 4;
			static constexpr uint16 edgeOriginVertexIndices[edgeOriginVertexIndexCount] = {0, 1, 3, 2};

			const uint16 indexA = edgeOriginVertexIndices[edgeIndex];
			const uint16 indexB = edgeOriginVertexIndices[(edgeIndex + 1) % edgeOriginVertexIndexCount];

			return {vertices[indexA], vertices[indexB]};
		}

		inline DualSnapEdge getDualSnapEdge(uint32 edgeIndex) const {
			static constexpr uint16 edgeOriginVertexIndexCount = 4;
			static constexpr uint16 edgeOriginVertexIndices[edgeOriginVertexIndexCount] = {0, 1, 3, 2};

			const uint16 indexA = edgeOriginVertexIndices[edgeIndex];
			const uint16 indexB = edgeOriginVertexIndices[(edgeIndex + 1) % edgeOriginVertexIndexCount];

			return {
				.vertexA = vertices[indexA],
				.vertexB = vertices[indexB],
				.uvA = textureCoordinates[indexA],
				.uvB = textureCoordinates[indexB],
			};
		}

		/// Vertices in world space, used in dual snapping.
		sequence<svec3<float32>> vertices;

		sequence<svec2<float32>> textureCoordinates;
		sequence<uint32> vertexIndices;
		sequence<uint32> textureCoordinateIndices;
		svec2<float32> scale;
	};

	struct StaticSurface {
		inline StaticEdge getEdge(uint32 edgeIndex) const {
			const uint16 indexA = edgeOriginVertexIndices[edgeIndex];
			const uint16 indexB = edgeOriginVertexIndices[(edgeIndex + 1) % edgeOriginVertexIndices.count()];

			return {vertices[indexA], vertices[indexB]};
		}

		inline DualSnapEdge getDualSnapEdge(vec3<float32> d) const {
			float32 maxDistance = min<float32>();
			uint32 edgeIndexA;
			uint32 edgeIndexB;
			uint32 vertexOffset = 0;

			while (vertexOffset < edgeOriginVertexIndices.count()) {
				const uint32 indexA = edgeOriginVertexIndices[vertexOffset];
				const uint32 indexB = edgeOriginVertexIndices[(vertexOffset + 1) % edgeOriginVertexIndices.count()];
				const vec3<float32> a = vertices[indexA];
				const vec3<float32> b = vertices[indexB];
				const vec3<float32> rightAB = cross(normal, normalized(a - b));
				const float32 distance = dot(rightAB, d);

				if (distance > maxDistance) {
					maxDistance = distance;
					edgeIndexA = indexA;
					edgeIndexB = indexB;
				}

				vertexOffset += 1;
			}

			return {
				.vertexA = vertices[edgeIndexA],
				.vertexB = vertices[edgeIndexB],
				.uvA = textureCoordinates[edgeIndexA],
				.uvB = textureCoordinates[edgeIndexB],
			};
		}

		/// Vertices in world space, used in single snapping.
		sequence<svec3<float32>> vertices;

		sequence<svec2<float32>> textureCoordinates;
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