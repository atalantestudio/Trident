// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

namespace Trident {
	inline void createHallScene(StaticSceneBuilder& builder) {
		builder.createStaticObject({
			.label = "hall_floor",
			.vertices = {
				{-16.0f,  0.0f, -16.0f},
				{-16.0f,  0.0f,  184.0f},
				{ 128.0f, 0.0f,  184.0f},
				{ 128.0f, 0.0f, -16.0f},
			},
			// .texture = "tile/tilefloor018a.jpg",
			// .normalMap = "tile/tilefloor018a_normal.jpg",
			.textureFiltering = {
				.translation = {0.25f, 0.625f},
				.rotation = 0.0f,
				.scale = {-0.25f, 0.25f},
			},
		});

		/* {
			"label": "hall_floor",
			"anchors": [-16, 0, -16, -16, 0, 184, 128, 0, 184],
			"texture": "tile/tilefloor018a.jpg",
			"normal_map": "tile/tilefloor018a_normal.jpg",
			"uv": [0.25, 0.625],
			"uv_rotation": 0,
			"uv_scale": [-0.25, 0.25]
		}, {
			"label": "hall_to_bike_room_floor_connector",
			"anchors": [-24, 0, 104, -24, 0, 152, -16, 0, 152],
			"texture": "tile/tilefloor018a.jpg",
			"normal_map": "tile/tilefloor018a_normal.jpg",
			"uv": [0.375, 0.125],
			"uv_rotation": 0,
			"uv_scale": [-0.25, 0.25]
		}, {
			"label": "bike_room_floor",
			"anchors": [-96, 0, 48, -96, 0, 184, -24, 0, 184],
			"texture": "tile/tilefloor018a.jpg",
			"normal_map": "tile/tilefloor018a_normal.jpg",
			"uv": [0.5, 0.624],
			"uv_rotation": 0,
			"uv_scale": [-0.25, 0.25]
		}, {
			"label": "hall_to_main_floor_connector",
			"anchors": [24, 0, 184, 24, 0, 192, 84, 0, 192],
			"texture": "tile/tilefloor018a.jpg",
			"normal_map": "tile/tilefloor018a_normal.jpg",
			"uv": [0.625, 0.5],
			"uv_rotation": 0,
			"uv_scale": [-0.25, 0.25]
		}, {
			"label": "main_floor",
			"anchors": [-168, 0, 192, -168, 0, 320, 128, 0, 320],
			"texture": "tile/tilefloor018a.jpg",
			"normal_map": "tile/tilefloor018a_normal.jpg",
			"uv": [0.625, 0.5],
			"uv_rotation": 0,
			"uv_scale": [-0.25, 0.25]
		} */
	}
}