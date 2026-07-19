// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	static uint32 findSortedIndex(const std::vector<float32>& elements, float32 element) {
		uint32 elementIndex = 0;

		while (elementIndex < elements.size()) {
			if (elements[elementIndex] >= element) {
				break;
			}

			elementIndex += 1;
		}

		return elementIndex;
	}

	static StaticSubsurface createSubsurface(SubsurfaceQuad quad) {
		return {
			.vertexComponents = {
				quad.start.x, -quad.start.y, 0.0f,
				quad.end.x, -quad.start.y, 0.0f,
				quad.start.x, -quad.end.y, 0.0f,
				quad.end.x, -quad.end.y, 0.0f,
			},
			.vertexTextureCoordinateComponents = {
				quad.start.x, quad.start.y,
				quad.end.x, quad.start.y,
				quad.start.x, quad.end.y,
				quad.end.x, quad.end.y,
			},
			.vertexIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.vertexTextureCoordinateIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.scale = quad.end - quad.start,
		};
	}

	void StaticSceneBuilder::carveSurface(StaticSurface& surface, const std::vector<Carving>& carvings, uint32& surfaceVertexCount) const {
		std::vector<float32> rowOffsets = {0.0f, surface.scale.y};
		SubsurfaceQuad quad;
		uint32 rowIndex;

		for (const Carving& carving : carvings) {
			rowIndex = findSortedIndex(rowOffsets, carving.offset.y);
			rowOffsets.insert(rowOffsets.begin() + rowIndex, carving.offset.y);

			rowIndex = findSortedIndex(rowOffsets, carving.offset.y + carving.size.y);
			rowOffsets.insert(rowOffsets.begin() + rowIndex, carving.offset.y + carving.size.y);
		}

		const uint32 rowCount = static_cast<uint32>(rowOffsets.size() - 1);

		sequence<std::vector<SubsurfaceQuad>> quadsPerRow(rowCount);
		sequence<std::vector<float32>> columnOffsetsPerRow(rowCount);

		rowIndex = 0;

		while (rowIndex < rowCount) {
			const float32 startY = rowOffsets[rowIndex];
			const float32 endY = rowOffsets[rowIndex + 1];

			std::vector<float32>& columnOffsets = columnOffsetsPerRow[rowIndex];

			bool excludeStart = false;
			bool excludeEnd = false;

			for (const Carving& carving : carvings) {
				if (
					carving.offset.y > startY || carving.offset.y + carving.size.y < endY
				) {
					continue;
				}

				uint32 columnIndex;

				if (carving.offset.x > 0.0f) {
					columnIndex = findSortedIndex(columnOffsets, carving.offset.x);
					columnOffsets.insert(columnOffsets.begin() + columnIndex, carving.offset.x);
				}
				else {
					excludeStart = true;
				}

				if (carving.offset.x + carving.size.x < surface.scale.x) {
					columnIndex = findSortedIndex(columnOffsets, carving.offset.x + carving.size.x);
					columnOffsets.insert(columnOffsets.begin() + columnIndex, carving.offset.x + carving.size.x);
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

				const StaticSubsurface& subsurface = createSubsurface(quad);

				surface.subsurfaces.emplace_back(subsurface);

				surfaceVertexCount += static_cast<uint32>(subsurface.vertexComponents.count() / 3);

				quadIndex += 1;
			}

			rowIndex += 1;
		}

		for (const SubsurfaceQuad& quad : quadsPerRow[rowIndex]) {
			const StaticSubsurface& subsurface = createSubsurface(quad);

			surface.subsurfaces.emplace_back(subsurface);

			surfaceVertexCount += static_cast<uint32>(subsurface.vertexComponents.count() / 3);
		}
	}
}