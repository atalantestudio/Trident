// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSurface.hpp"

namespace Trident {
	struct Room {
		StaticSurface& support(vec3<float32> direction) {
			float32 maxDistance = min<float32>();
			uint32 maxSurfaceIndex = max<uint32>();
			uint32 surfaceIndex = 0;

			while (surfaceIndex < surfaces.count()) {
				const StaticSurface& surface = surfaces[surfaceIndex];

				// TODO

				surfaceIndex += 1;
			}

			return surfaces[maxSurfaceIndex];
		}

		uint32 floorLevelIndex;
		svec2<float32> position;

		sequence<StaticSurface> surfaces;
	};

	struct RectangularRoom : public Room {
		explicit RectangularRoom(svec2<float32> position, svec2<float32> size, float32 floorLevel, float32 ceilingLevel, svec2<float32> luxelScale) {
			// TODO(test)
			constexpr float32 m1 = 1.0f / 256.0f;

			const float32 positionX = position.x;
			const float32 positionZ = position.y;
			const float32 sizeX = size.x;
			const float32 sizeY = ceilingLevel - floorLevel;
			const float32 sizeZ = size.y;
			const float32 halfSizeX = sizeX * 0.5f;
			const float32 halfSizeY = sizeY * 0.5f;
			const float32 halfSizeZ = sizeZ * 0.5f;

			surfaces = {
				// Floor (towards +Y)
				{
					.materialIndex = 0,
					.transform = {
						.position = {positionX, floorLevel, positionZ},
						.rotation = {0.0f, 0.0f, 0.0f},
						.scale = {sizeX, sizeZ},
					},
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 / 0.25f, m1 / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				},

				// Left wall (towards +X)
				{
					.materialIndex = 0,
					.transform = {
						.position = {positionX - halfSizeX, floorLevel + halfSizeY, positionZ},
						.rotation = {0.0f, pi<float32>() * 0.5f, pi<float32>() * 0.5f},
						.scale = {sizeZ, sizeY},
					},
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 / 0.25f, m1 / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				},

				// Right wall (towards -X)
				{
					.materialIndex = 0,
					.transform = {
						.position = {positionX + halfSizeX, floorLevel + halfSizeY, positionZ},
						.rotation = {0.0f, -pi<float32>() * 0.5f, -pi<float32>() * 0.5f},
						.scale = {sizeZ, sizeY},
					},
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 / 0.25f, m1 / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				},

				// Back wall (towards +Z)
				{
					.materialIndex = 0,
					.transform = {
						.position = {positionX, floorLevel + halfSizeY, positionZ - halfSizeZ},
						.rotation = {-pi<float32>() * 0.5f, 0.0f, pi<float32>()},
						.scale = {sizeX, sizeY},
					},
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 / 0.25f, m1 / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				},

				// Front wall (towards -Z)
				{
					.materialIndex = 0,
					.transform = {
						.position = {positionX, floorLevel + halfSizeY, positionZ + halfSizeZ},
						.rotation = {pi<float32>() * 0.5f, 0.0f, 0.0f},
						.scale = {sizeX, sizeY},
					},
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 / 0.25f, m1 / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				},

				// TODO: Ceiling (towards -Y)
				/* {
					.materialIndex = 0,
					.transform = {
						.position = {positionX, ceilingLevel, positionZ},
						.rotation = {0.0f, 0.0f, pi<float32>()},
						.scale = {sizeX, sizeZ},
					},
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 / 0.25f, m1 / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				}, */
			};
		}
	};
}