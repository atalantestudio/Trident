// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class HallReferenceMap final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_hall_ref";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr LightmapDescriptor LIGHTMAP = {
					.luxelScale = {1.0f, 1.0f},
					.emissiveColor = {0.0f, 0.0f, 0.0f},
					.emissiveIntensity = 0.0f,
					.albedo = {1.0f, 1.0f, 1.0f},
				};
				static constexpr vec3<float32> VERTICES[] = {
					{-16.0f,   0.0f,    184.0f},
					{ 128.0f,  0.0f,    184.0f},
					{ 128.0f,  0.0f,   -16.0f },
					{-16.0f,   0.0f,   -16.0f },
					{-24.0f,   0.0f,    152.0f},
					{-16.0f,   0.0f,    152.0f},
					{-16.0f,   0.0f,    104.0f},
					{-24.0f,   0.0f,    104.0f},
					{-96.0f,   0.0f,    184.0f},
					{-24.0f,   0.0f,    184.0f},
					{-24.0f,   0.0f,    48.0f },
					{-96.0f,   0.0f,    48.0f },
					{ 24.0f,   0.0f,    192.0f},
					{ 84.0f,   0.0f,    192.0f},
					{ 84.0f,   0.0f,    184.0f},
					{ 24.0f,   0.0f,    184.0f},
					{-168.0f,  0.0f,    320.0f},
					{ 128.0f,  0.0f,    320.0f},
					{ 128.0f,  0.0f,    192.0f},
					{-168.0f,  0.0f,    192.0f},
					{ 128.0f,  128.0f, -16.0f },
					{ 92.0f,   128.0f, -16.0f },
					{ 92.0f,   0.0f,   -16.0f },
					{ 128.0f,  0.0f,   -16.0f },
					{ 36.0f,   128.0f, -16.0f },
					{-16.0f,   128.0f, -16.0f },
					{-16.0f,   0.0f,   -16.0f },
					{ 36.0f,   0.0f,   -16.0f },
					{-16.0f,   128.0f, -16.0f },
					{-16.0f,   128.0f,  100.0f},
					{-16.0f,   0.0f,    100.0f},
					{-16.0f,   0.0f,   -16.0f },
					{-16.0f,   128.0f,  156.0f},
					{-16.0f,   128.0f,  184.0f},
					{-16.0f,   0.0f,    184.0f},
					{-16.0f,   0.0f,    156.0f},
					{-16.0f,   128.0f,  184.0f},
					{ 20.0f,   128.0f,  184.0f},
					{ 20.0f,   0.0f,    184.0f},
					{-16.0f,   0.0f,    184.0f},
					{ 88.0f,   128.0f,  184.0f},
					{ 128.0f,  128.0f,  184.0f},
					{ 128.0f,  0.0f,    184.0f},
					{ 88.0f,   0.0f,    184.0f},
					{ 128.0f,  128.0f,  184.0f},
					{ 128.0f,  128.0f, -16.0f },
					{ 128.0f,  0.0f,   -16.0f },
					{ 128.0f,  0.0f,    184.0f},
					{-24.0f,   128.0f,  48.0f },
					{-96.0f,   128.0f,  48.0f },
					{-96.0f,   0.0f,    48.0f },
					{-24.0f,   0.0f,    48.0f },
					{-96.0f,   128.0f,  48.0f },
					{-96.0f,   128.0f,  184.0f},
					{-96.0f,   0.0f,    184.0f},
					{-96.0f,   0.0f,    48.0f },
					{-96.0f,   128.0f,  184.0f},
					{-24.0f,   128.0f,  184.0f},
					{-24.0f,   0.0f,    184.0f},
					{-96.0f,   0.0f,    184.0f},
					{-24.0f,   128.0f,  184.0f},
					{-24.0f,   128.0f,  156.0f},
					{-24.0f,   0.0f,    156.0f},
					{-24.0f,   0.0f,    184.0f},
					{-24.0f,   128.0f,  100.0f},
					{-24.0f,   128.0f,  48.0f },
					{-24.0f,   0.0f,    48.0f },
					{-24.0f,   0.0f,    100.0f},
					{ 92.0f,   128.0f, -16.0f },
					{ 36.0f,   128.0f, -16.0f },
					{ 36.0f,   112.0f, -16.0f },
					{ 92.0f,   112.0f, -16.0f },
					{-16.0f,   128.0f,  100.0f},
					{-16.0f,   128.0f,  156.0f},
					{-16.0f,   112.0f,  156.0f},
					{-16.0f,   112.0f,  100.0f},
					{ 20.0f,   128.0f,  184.0f},
					{ 88.0f,   128.0f,  184.0f},
					{ 88.0f,   112.0f,  184.0f},
					{ 20.0f,   112.0f,  184.0f},
					{-24.0f,   128.0f,  156.0f},
					{-24.0f,   128.0f,  100.0f},
					{-24.0f,   112.0f,  100.0f},
					{-24.0f,   112.0f,  156.0f},
					{ 128.0f,  240.0f, -16.0f },
					{-16.0f,   240.0f, -16.0f },
					{-16.0f,   128.0f, -16.0f },
					{ 128.0f,  128.0f, -16.0f },
					{-16.0f,   240.0f, -16.0f },
					{-16.0f,   240.0f,  184.0f},
					{-16.0f,   128.0f,  184.0f},
					{-16.0f,   128.0f, -16.0f },
					{-16.0f,   240.0f,  184.0f},
					{ 128.0f,  240.0f,  184.0f},
					{ 128.0f,  128.0f,  184.0f},
					{-16.0f,   128.0f,  184.0f},
					{ 128.0f,  240.0f,  184.0f},
					{ 128.0f,  240.0f, -16.0f },
					{ 128.0f,  128.0f, -16.0f },
					{ 128.0f,  128.0f,  184.0f},
					{-16.0f,   240.0f, -16.0f },
					{ 128.0f,  240.0f, -16.0f },
					{ 128.0f,  240.0f,  184.0f},
					{-16.0f,   240.0f,  184.0f},
					{-96.0f,   128.0f,  48.0f },
					{-24.0f,   128.0f,  48.0f },
					{-24.0f,   128.0f,  184.0f},
					{-96.0f,   128.0f,  184.0f},
					{-16.0f,   108.0f,  100.0f},
					{-16.0f,   108.0f,  104.0f},
					{-16.0f,   0.0f,    104.0f},
					{-16.0f,   0.0f,    100.0f},
					{-16.0f,   108.0f,  152.0f},
					{-16.0f,   108.0f,  156.0f},
					{-16.0f,   0.0f,    156.0f},
					{-16.0f,   0.0f,    152.0f},
					{-16.0f,   112.0f,  100.0f},
					{-16.0f,   112.0f,  156.0f},
					{-16.0f,   108.0f,  156.0f},
					{-16.0f,   108.0f,  100.0f},
					{-24.0f,   108.0f,  104.0f},
					{-24.0f,   108.0f,  100.0f},
					{-24.0f,   0.0f,    100.0f},
					{-24.0f,   0.0f,    104.0f},
					{-24.0f,   108.0f,  156.0f},
					{-24.0f,   108.0f,  152.0f},
					{-24.0f,   0.0f,    152.0f},
					{-24.0f,   0.0f,    156.0f},
					{-24.0f,   112.0f,  156.0f},
					{-24.0f,   112.0f,  100.0f},
					{-24.0f,   108.0f,  100.0f},
					{-24.0f,   108.0f,  156.0f},
					{-16.0f,   108.0f,  104.0f},
					{-24.0f,   108.0f,  104.0f},
					{-24.0f,   0.0f,    104.0f},
					{-16.0f,   0.0f,    104.0f},
					{-24.0f,   108.0f,  152.0f},
					{-16.0f,   108.0f,  152.0f},
					{-16.0f,   0.0f,    152.0f},
					{-24.0f,   0.0f,    152.0f},
					{-24.0f,   108.0f,  100.0f},
					{-16.0f,   108.0f,  100.0f},
					{-16.0f,   108.0f,  152.0f},
					{-24.0f,   108.0f,  152.0f},
				};
				static constexpr view<vec3<float32>> HALL_FLOOR_VERTICES(&VERTICES[0], 4);
				static constexpr view<vec3<float32>> HALL_TO_BIKE_ROOM_FLOOR_CONNECTOR_VERTICES(&VERTICES[4], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_FLOOR_VERTICES(&VERTICES[8], 4);
				static constexpr view<vec3<float32>> HALL_TO_MAIN_FLOOR_CONNECTOR_VERTICES(&VERTICES[12], 4);
				static constexpr view<vec3<float32>> MAIN_FLOOR_VERTICES(&VERTICES[16], 4);
				static constexpr view<vec3<float32>> HALL_ENTRANCE_LEFT_WALL_VERTICES(&VERTICES[20], 4);
				static constexpr view<vec3<float32>> HALL_ENTRANCE_RIGHT_WALL_VERTICES(&VERTICES[24], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_ENTRANCE_LEFT_WALL_VERTICES(&VERTICES[28], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_ENTRANCE_RIGHT_WALL_VERTICES(&VERTICES[32], 4);
				static constexpr view<vec3<float32>> MAIN_ENTRANCE_LEFT_WALL_VERTICES(&VERTICES[36], 4);
				static constexpr view<vec3<float32>> MAIN_ENTRANCE_RIGHT_WALL_VERTICES(&VERTICES[40], 4);
				static constexpr view<vec3<float32>> MAILBOX_WALL_VERTICES(&VERTICES[44], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_BACK_WALL_VERTICES(&VERTICES[48], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_LEFT_WALL_VERTICES(&VERTICES[52], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_FRONT_WALL_VERTICES(&VERTICES[56], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_RIGHT_WALL_LEFT_VERTICES(&VERTICES[60], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_RIGHT_WALL_RIGHT_VERTICES(&VERTICES[64], 4);
				static constexpr view<vec3<float32>> HALL_BACK_DOOR_WALL_VERTICES(&VERTICES[68], 4);
				static constexpr view<vec3<float32>> HALL_LEFT_DOOR_WALL_VERTICES(&VERTICES[72], 4);
				static constexpr view<vec3<float32>> HALL_FRONT_DOOR_WALL_VERTICES(&VERTICES[76], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_RIGHT_DOOR_WALL_VERTICES(&VERTICES[80], 4);
				static constexpr view<vec3<float32>> HALL_BACK_WALL_TOP_VERTICES(&VERTICES[84], 4);
				static constexpr view<vec3<float32>> HALL_LEFT_WALL_TOP_VERTICES(&VERTICES[88], 4);
				static constexpr view<vec3<float32>> HALL_FRONT_WALL_TOP_VERTICES(&VERTICES[92], 4);
				static constexpr view<vec3<float32>> HALL_RIGHT_WALL_TOP_VERTICES(&VERTICES[96], 4);
				static constexpr view<vec3<float32>> HALL_ROOM_CEILING_VERTICES(&VERTICES[100], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_CEILING_VERTICES(&VERTICES[104], 4);
				static constexpr view<vec3<float32>> HALL_ROOM_TO_BIKE_ROOM_LEFT_DOOR_FRAME_VERTICES(&VERTICES[108], 4);
				static constexpr view<vec3<float32>> HALL_ROOM_TO_BIKE_ROOM_RIGHT_DOOR_FRAME_VERTICES(&VERTICES[112], 4);
				static constexpr view<vec3<float32>> HALL_ROOM_TO_BIKE_ROOM_TOP_DOOR_FRAME_VERTICES(&VERTICES[116], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_TO_HALL_ROOM_LEFT_DOOR_FRAME_VERTICES(&VERTICES[120], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_TO_HALL_ROOM_RIGHT_DOOR_FRAME_VERTICES(&VERTICES[124], 4);
				static constexpr view<vec3<float32>> BIKE_ROOM_TO_HALL_ROOM_TOP_DOOR_FRAME_VERTICES(&VERTICES[128], 4);
				static constexpr view<vec3<float32>> BIKE_ENTRANCE_LEFT_DOOR_FRAME_VERTICES(&VERTICES[132], 4);
				static constexpr view<vec3<float32>> BIKE_ENTRANCE_RIGHT_DOOR_FRAME_VERTICES(&VERTICES[136], 4);
				static constexpr view<vec3<float32>> BIKE_ENTRANCE_TOP_DOOR_FRAME_VERTICES(&VERTICES[140], 4);

				// Texture scale multipliers
				static constexpr float32 m1 = 256.0f;
				static constexpr float32 m2 = 512.0f;

				MapBuilder builder({
					.skyColor = {0.0f, 0.0f, 0.0f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("overcast_soil_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {100.0f, 0.0f, 32.0f},
						.viewDirection = normalized(vec3<float32>{radians(-40.0_deg), 0.0f, radians(50.0_deg)}),
					},
				});

				const MaterialReference tileFloorMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_tile_floor"));
				const MaterialReference plasterWallMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_plaster_wall"));
				const MaterialReference woodDoorMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_wood_door"));
				const MaterialReference plasterCeilingMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_plaster_ceiling"));

				// Floor
				builder.createFace({
					.vertices = HALL_FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = tileFloorMaterial,
					.textureFiltering = {
						.translation = {0.25f, 0.25f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m1 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_TO_BIKE_ROOM_FLOOR_CONNECTOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = tileFloorMaterial,
					.textureFiltering = {
						.translation = {0.375f, 0.75f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m1 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = tileFloorMaterial,
					.textureFiltering = {
						.translation = {0.5f, 0.25f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m1 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_TO_MAIN_FLOOR_CONNECTOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = tileFloorMaterial,
					.textureFiltering = {
						.translation = {0.625f, 0.125f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m1 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = MAIN_FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = tileFloorMaterial,
					.textureFiltering = {
						.translation = {0.625f, 0.125f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m1 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});

				// Walls
				builder.createFace({
					.vertices = HALL_ENTRANCE_LEFT_WALL_VERTICES,
					.normal = {0.0f, 0.0f, 1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.281f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_ENTRANCE_RIGHT_WALL_VERTICES,
					.normal = {0.0f, 0.0f, 1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.281f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_ENTRANCE_LEFT_WALL_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.39f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_ENTRANCE_RIGHT_WALL_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.36f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = MAIN_ENTRANCE_LEFT_WALL_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.125f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = MAIN_ENTRANCE_RIGHT_WALL_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.312f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = MAILBOX_WALL_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.262f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_BACK_WALL_VERTICES,
					.normal = {0.0f, 0.0f, 1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.812f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_LEFT_WALL_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.625f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_FRONT_WALL_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.75f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_RIGHT_WALL_LEFT_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.58f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_RIGHT_WALL_RIGHT_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.295f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_BACK_DOOR_WALL_VERTICES,
					.normal = {0.0f, 0.0f, 1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.71f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_LEFT_DOOR_WALL_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.36f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_FRONT_DOOR_WALL_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.53f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_RIGHT_DOOR_WALL_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = plasterWallMaterial,
					.textureFiltering = {
						.translation = {0.797f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_BACK_WALL_TOP_VERTICES,
					.normal = {0.0f, 0.0f, 1.0f},
					.material = plasterCeilingMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.125f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_LEFT_WALL_TOP_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = plasterCeilingMaterial,
					.textureFiltering = {
						.translation = {0.125f, 0.125f},
						.rotation = 0.0f,
						.scale = {m2 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_FRONT_WALL_TOP_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = plasterCeilingMaterial,
					.textureFiltering = {
						.translation = {0.125f, 0.125f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_RIGHT_WALL_TOP_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = plasterCeilingMaterial,
					.textureFiltering = {
						.translation = {0.686f, 0.125f},
						.rotation = 0.0f,
						.scale = {m2 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});

				// Ceiling
				builder.createFace({
					.vertices = HALL_ROOM_CEILING_VERTICES,
					.normal = {0.0f, -1.0f, 0.0f},
					.material = plasterCeilingMaterial,
					.textureFiltering = {
						.translation = {0.498f, 0.875f},
						.rotation = 0.5f,
						.scale = {m2 * 0.25f, m2 * -0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_CEILING_VERTICES,
					.normal = {0.0f, -1.0f, 0.0f},
					.material = plasterCeilingMaterial,
					.textureFiltering = {
						.translation = {0.69f, 0.258f},
						.rotation = 0.5f,
						.scale = {m2 * 0.25f, m2 * -0.25f},
					},
					.lightmap = LIGHTMAP,
				});

				// Door frames
				builder.createFace({
					.vertices = HALL_ROOM_TO_BIKE_ROOM_LEFT_DOOR_FRAME_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.875f, 0.155f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_ROOM_TO_BIKE_ROOM_RIGHT_DOOR_FRAME_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.0625f, 0.155f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = HALL_ROOM_TO_BIKE_ROOM_TOP_DOOR_FRAME_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {-0.125f, 0.125f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_TO_HALL_ROOM_LEFT_DOOR_FRAME_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.155f},
						.rotation = 0.0f,
						.scale = {m1 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_TO_HALL_ROOM_RIGHT_DOOR_FRAME_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.8125f, 0.155f},
						.rotation = 0.0f,
						.scale = {m1 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ROOM_TO_HALL_ROOM_TOP_DOOR_FRAME_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.125f},
						.rotation = 0.0f,
						.scale = {m1 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ENTRANCE_LEFT_DOOR_FRAME_VERTICES,
					.normal = {0.0f, 0.0f, 1.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.195f, 0.1575f},
						.rotation = 0.0f,
						.scale = {m1 * -0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ENTRANCE_RIGHT_DOOR_FRAME_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.0625f, 0.1575f},
						.rotation = 0.0f,
						.scale = {m1 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});
				builder.createFace({
					.vertices = BIKE_ENTRANCE_TOP_DOOR_FRAME_VERTICES,
					.normal = {0.0f, -1.0f, 0.0f},
					.material = woodDoorMaterial,
					.textureFiltering = {
						.translation = {0.19f, -0.017f},
						.rotation = 1.0f,
						.scale = {m1 * 0.25f, m2 * 0.25f},
					},
					.lightmap = LIGHTMAP,
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}