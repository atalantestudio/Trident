// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2DType.hpp"
#include "Engine/ECS/Entity.hpp"

namespace Trident {
	class CachedGeometry2D {
		private:
			static void copyVertexComponentsAndIndices(std::vector<float32>& vertexComponents, std::vector<uint16>& indices, view<float32> sourceVertexComponents, view<uint16> sourceTriangleListIndices, view<uint16> sourceLineStripIndices) {
				std::size_t previousSize = vertexComponents.size();

				vertexComponents.resize(previousSize + sourceVertexComponents.count());

				std::copy(sourceVertexComponents.begin(), sourceVertexComponents.end(), &vertexComponents[previousSize]);

				previousSize = indices.size();

				indices.resize(previousSize + sourceTriangleListIndices.count() + sourceLineStripIndices.count());

				std::copy(sourceTriangleListIndices.begin(), sourceTriangleListIndices.end(), &indices[previousSize]);
				std::copy(sourceLineStripIndices.begin(), sourceLineStripIndices.end(), &indices[previousSize + sourceTriangleListIndices.count()]);
			}

		public:
			static CachedGeometry2D unitCircle(Entity entity, uint32 resolution, std::vector<float32>& vertexComponents, std::vector<uint16>& indices) {
				// TODO: Negative radii are not allowed.
				// ENGINE_ASSERT(descriptor.radius > 0.0f, "The radius must be positive.");

				ENGINE_ASSERT(resolution > 2, "The resolution must be greater than 2.");

				const CachedGeometry2D geometry = {
					.entity = entity,
					.centroid = {0.0_m, 0.0_m},
					.radius = 0.5_m,
					.vertexOffset = static_cast<uint32>(vertexComponents.size() / 2),
					.vertexCount = resolution,
					.indexOffset = static_cast<uint32>(indices.size()),
					.triangleListIndexCount = static_cast<uint16>(resolution * 3),
					.lineStripIndexCount = static_cast<uint16>(resolution + 1),
					.type = Geometry2DType::CIRCLE,
				};

				const float32 theta = pi<float32>() * 2.0f / resolution;

				sequence<float32> vertices = sequence<float32>(resolution * 2);
				sequence<uint16> triangleListIndices = sequence<uint16>(geometry.triangleListIndexCount);
				sequence<uint16> lineStripIndices = sequence<uint16>(geometry.lineStripIndexCount);

				float32 angle = 0.0f;
				uint32 triangleIndex = 0;
				vec2<float32> vertex;

				while (triangleIndex < resolution) {
					vertex = {sin(angle), cos(angle)};
					vertex *= geometry.radius;

					vertices[triangleIndex * 2 + 0] = vertex[0];
					vertices[triangleIndex * 2 + 1] = vertex[1];

					triangleListIndices[triangleIndex * 3 + 0] = 0;
					triangleListIndices[triangleIndex * 3 + 1] = triangleIndex + 1;
					triangleListIndices[triangleIndex * 3 + 2] = triangleIndex + 2;

					lineStripIndices[triangleIndex] = triangleIndex;

					angle += theta;
					triangleIndex += 1;
				}

				lineStripIndices[triangleIndex] = 0;

				copyVertexComponentsAndIndices(vertexComponents, indices, vertices, triangleListIndices, lineStripIndices);

				return geometry;
			}

			/// 
			///         0
			///    0 ‒‒‒‒‒‒‒ 1
			///    ¦         ¦
			/// 3  ¦         ¦  1
			///    ¦         ¦
			///    3 ‒‒‒‒‒‒‒ 2
			///         2
			/// 
			static CachedGeometry2D unitSquare(Entity entity, std::vector<float32>& vertexComponents, std::vector<uint16>& indices) {
				CachedGeometry2D geometry = {
					.entity = entity,
					.centroid = {0.0_m, 0.0_m},
					.radius = 0.0_m,
					.vertexOffset = static_cast<uint32>(vertexComponents.size() / 2),
					.vertexCount = 4,
					.indexOffset = static_cast<uint32>(indices.size()),
					.triangleListIndexCount = 6,
					.lineStripIndexCount = 5,
					.type = Geometry2DType::CONVEX_HULL,
				};

				const vec2<float32> centroid = {geometry.centroid[0], geometry.centroid[1]};
				const sequence<float32> vertices = {
					-0.5f,  0.5f, // Top left
					 0.5f,  0.5f, // Top right
					 0.5f, -0.5f, // Bottom right
					-0.5f, -0.5f, // Bottom left
				};
				const sequence<uint16> triangleListIndices = {
					0, 1, 2,
					0, 2, 3,
				};
				const sequence<uint16> lineStripIndices = {0, 1, 2, 3, 0};

				uint32 vertexOffset = 0;
				vec2<float32> vertex;

				while (vertexOffset < vertices.count()) {
					vertex = {vertices[vertexOffset], vertices[vertexOffset + 1]};
					geometry.radius = max(geometry.radius, distanceSquared(vertex, centroid));

					vertexOffset += 2;
				}

				geometry.radius = sqrt(geometry.radius);

				copyVertexComponentsAndIndices(vertexComponents, indices, vertices, triangleListIndices, lineStripIndices);

				return geometry;
			}

			static CachedGeometry2D convexHull(Entity entity, view<float32> sourceVertexComponents, view<uint16> sourceTriangleListIndices, view<uint16> sourceLineStripIndices, std::vector<float32>& vertexComponents, std::vector<uint16>& indices) {
				CachedGeometry2D geometry = {
					.entity = entity,
					.centroid = {0.0_m, 0.0_m},
					.radius = 0.0_m,
					.vertexOffset = static_cast<uint32>(vertexComponents.size() / 2),
					.vertexCount = static_cast<uint32>(sourceVertexComponents.count()),
					.indexOffset = static_cast<uint32>(indices.size()),
					.triangleListIndexCount = static_cast<uint16>(sourceTriangleListIndices.count()),
					.lineStripIndexCount = static_cast<uint16>(sourceLineStripIndices.count()),
					.type = Geometry2DType::CONVEX_HULL,
				};

				uint32 vertexOffset = 0;
				vec2<float32> vertex;

				while (vertexOffset < sourceVertexComponents.count()) {
					vertex = {sourceVertexComponents[vertexOffset], sourceVertexComponents[vertexOffset + 1]};
					geometry.centroid += {sourceVertexComponents[vertexOffset], sourceVertexComponents[vertexOffset + 1]};

					vertexOffset += 2;
				}

				geometry.centroid /= static_cast<float32>(sourceVertexComponents.count());

				const vec2<float32> centroid = {geometry.centroid[0], geometry.centroid[1]};

				while (vertexOffset < sourceVertexComponents.count()) {
					vertex = {sourceVertexComponents[vertexOffset], sourceVertexComponents[vertexOffset + 1]};
					geometry.radius = max(geometry.radius, distanceSquared(vertex, centroid));

					vertexOffset += 2;
				}

				geometry.radius = sqrt(geometry.radius);

				copyVertexComponentsAndIndices(vertexComponents, indices, sourceVertexComponents, sourceTriangleListIndices, sourceLineStripIndices);

				return geometry;
			}

		public:
			Entity entity;
			svec2<float32> centroid;
			float32 radius;
			uint32 vertexOffset;
			uint32 vertexCount;
			uint32 indexOffset;
			uint16 triangleListIndexCount;
			uint16 lineStripIndexCount;
			Geometry2DType type;
	};
}