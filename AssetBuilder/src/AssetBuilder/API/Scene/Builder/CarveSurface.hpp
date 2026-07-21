// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	static void emplaceSorted(std::vector<float32>& vector, float32 a) {
		index i = 0;

		while (i < vector.size()) {
			const float32 b = vector[i];

			if (a == b) {
				return;
			}

			if (b > a) {
				vector.insert(vector.begin() + i, a);

				return;
			}

			i += 1;
		}

		vector.emplace_back(a);
	}

	static StaticSubsurface createSubsurface(SubsurfaceQuad quad, svec3<float32> origin, svec2<float32> surfaceScale) {
		DEBUG_ASSERT(quad.end.x > quad.start.x && quad.end.y > quad.start.y);

		return {
			.vertices = {
				{origin.x + quad.start.x, origin.y - quad.start.y, origin.z},
				{origin.x + quad.end.x, origin.y - quad.start.y, origin.z},
				{origin.x + quad.start.x, origin.y - quad.end.y, origin.z},
				{origin.x + quad.end.x, origin.y - quad.end.y, origin.z},
			},
			.textureCoordinates = {
				{quad.start.x / surfaceScale.x, quad.start.y / surfaceScale.y},
				{quad.end.x / surfaceScale.x, quad.start.y / surfaceScale.y},
				{quad.start.x / surfaceScale.x, quad.end.y / surfaceScale.y},
				{quad.end.x / surfaceScale.x, quad.end.y / surfaceScale.y},
			},
			.vertexIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.textureCoordinateIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.scale = quad.end - quad.start,
		};
	}

	void StaticSceneBuilder::carveSurface(StaticSurface& surface, svec3<float32> origin, const std::vector<Carving>& carvings, uint32& surfaceVertexCount) const {
		std::vector<float32> rowOffsets = {0.0f, surface.scale.y};
		SubsurfaceQuad quad;

		for (const Carving& carving : carvings) {
			emplaceSorted(rowOffsets, carving.offset.y);
			emplaceSorted(rowOffsets, carving.offset.y + carving.size.y);

			quad.start = carving.offset;
			quad.end = carving.offset + carving.size;

			surface.hollowSubsurfaces.emplace_back(createSubsurface(quad, origin, surface.scale));
		}

		const uint32 rowCount = static_cast<uint32>(rowOffsets.size() - 1);

		sequence<std::vector<SubsurfaceQuad>> quadsPerRow(rowCount);
		sequence<std::vector<float32>> columnOffsetsPerRow(rowCount);
		uint32 rowIndex = 0;

		while (rowIndex < rowCount) {
			const float32 startY = rowOffsets[rowIndex];
			const float32 endY = rowOffsets[rowIndex + 1];

			std::vector<float32>& columnOffsets = columnOffsetsPerRow[rowIndex];

			bool excludeStart = false;
			bool excludeEnd = false;

			for (const Carving& carving : carvings) {
				if (carving.offset.y > startY || carving.offset.y + carving.size.y < endY) {
					continue;
				}

				if (carving.offset.x > 0.0f) {
					emplaceSorted(columnOffsets, carving.offset.x);
				}
				else {
					excludeStart = true;
				}

				if (carving.offset.x + carving.size.x < surface.scale.x) {
					emplaceSorted(columnOffsets, carving.offset.x + carving.size.x);
				}
				else {
					excludeEnd = true;
				}
			}

			if (!excludeStart) {
				columnOffsets.insert(columnOffsets.begin(), 0.0f);
			}

			if (!excludeEnd) {
				columnOffsets.insert(columnOffsets.end(), surface.scale.x);
			}

			rowIndex += 1;
		}

		rowIndex = 0;

		while (rowIndex < rowCount) {
			const std::vector<float32>& columnOffsets = columnOffsetsPerRow[rowIndex];

			quad.start.y = rowOffsets[rowIndex];
			quad.end.y = rowOffsets[rowIndex + 1];

			std::vector<SubsurfaceQuad>& quads = quadsPerRow[rowIndex];
			uint32 columnIndex = 0;

			while (columnIndex < columnOffsets.size() - 1) {
				quad.start.x = columnOffsets[columnIndex];
				quad.end.x = columnOffsets[columnIndex + 1];

				quads.emplace_back(quad);

				columnIndex += 2;
			}

			rowIndex += 1;
		}

		rowIndex = 0;

		while (rowIndex < rowCount - 1) {
			std::vector<SubsurfaceQuad>& quads = quadsPerRow[rowIndex];

			uint32 quadIndex = 0;

			while (quadIndex < quads.size()) {
				SubsurfaceQuad& quad = quads[quadIndex];

				uint32 belowRowIndex = rowIndex;

				while (belowRowIndex < rowCount - 1) {
					std::vector<SubsurfaceQuad>& belowQuads = quadsPerRow[belowRowIndex];

					sint32 belowQuadIndex = static_cast<sint32>(belowQuads.size()) - 1;

					while (belowQuadIndex >= 0) {
						const SubsurfaceQuad& belowQuad = belowQuads[belowQuadIndex];

						if (quad.start.x == belowQuad.start.x && quad.end.x == belowQuad.end.x && quad.end.y == belowQuad.start.y) {
							quad.end.y = belowQuad.end.y;

							belowQuads.erase(belowQuads.begin() + belowQuadIndex);
						}

						belowQuadIndex -= 1;
					}

					belowRowIndex += 1;
				}

				const StaticSubsurface& subsurface = createSubsurface(quad, origin, surface.scale);

				surface.subsurfaces.emplace_back(subsurface);

				surfaceVertexCount += static_cast<uint32>(subsurface.vertices.count());

				quadIndex += 1;
			}

			rowIndex += 1;
		}

		for (const SubsurfaceQuad& quad : quadsPerRow[rowIndex]) {
			const StaticSubsurface& subsurface = createSubsurface(quad, origin, surface.scale);

			surface.subsurfaces.emplace_back(subsurface);

			surfaceVertexCount += static_cast<uint32>(subsurface.vertices.count());
		}
	}
}