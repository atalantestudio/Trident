// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class HallScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_hall";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				// Texture scale multipliers
				static constexpr float32 m1 = 256.0f;
				static constexpr float32 m2 = 512.0f;

				SceneBuilder builder({
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});

				const svec3<float32> upRotation = {0.0f, 0.0f, 0.0f};
				const svec3<float32> downRotation = {0.0f, 0.0f, pi<float32>()};
				const svec3<float32> leftRotation = {0.0f, -pi<float32>() * 0.5f, -pi<float32>() * 0.5f};
				const svec3<float32> rightRotation = {0.0f, pi<float32>() * 0.5f, pi<float32>() * 0.5f};
				const svec3<float32> forwardRotation = {-pi<float32>() * 0.5f, 0.0f, pi<float32>()};
				const svec3<float32> backwardRotation = {pi<float32>() * 0.5f, 0.0f, 0.0f};

				// TODO(asset-builder/lightmapper): For now the materials must be loaded in the order they are used when placing static surfaces and props.
				const MaterialReference tileFloorMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("tile_floor"));
				const MaterialReference plasterWallMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("plaster_wall"));
				const MaterialReference woodDoorMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("wood_door"));
				const MaterialReference plasterCeilingMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("plaster_ceiling"));

				/// Floors

				// Hall floor
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({144.0f, 200.0f}),
						.material = tileFloorMaterial,
						.textureFiltering = {
							.translation = {0.25f, 0.625f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m1 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {56.0f, 0.0f, 84.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Hall to bike room floor connector
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({8.0f, 48.0f}),
						.material = tileFloorMaterial,
						.textureFiltering = {
							.translation = {0.375f, 0.125f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m1 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-20.0f, 0.0f, 128.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Bike room floor
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({72.0f, 136.0f}),
						.material = tileFloorMaterial,
						.textureFiltering = {
							.translation = {0.5f, 0.625f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m1 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-60.0f, 0.0f, 116.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Hall to main floor connector
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({60.0f, 8.0f}),
						.material = tileFloorMaterial,
						.textureFiltering = {
							.translation = {0.625f, 0.5f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m1 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {54.0f, 0.0f, 188.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Main floor
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({296.0f, 128.0f}),
						.material = tileFloorMaterial,
						.textureFiltering = {
							.translation = {0.625f, 0.5f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m1 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-20.0f, 0.0f, 256.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				/// Walls

				// Hall entrance left wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({52.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.281f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {10.0f, 64.0f, -16.0f},
						.axis = {0.0f, 1.0f, 0.0},
						.angle = radians(180.0_deg),
					}
				);

				// Hall entrance right wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({36.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {110.0f, 64.0f, -16.0f},
						.axis = {0.0f, 1.0f, 0.0},
						.angle = radians(180.0_deg),
					}
				);

				// Bike room entrance left wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({116.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.39f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-16.0f, 64.0f, 42.0f},
						.axis =  {0.0f, 1.0f, 0.0},
						.angle = radians(90.0_deg),
					}
				);

				// Bike room entrance right wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({28.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.36f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-16.0f, 64.0f, 170.0f},
						.axis =  {0.0f, 1.0f, 0.0},
						.angle = radians(90.0_deg),
					}
				);

				// Main entrance left wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({36.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.125f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {2.0f, 64.0f, 184.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				// Main entrance right wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({40.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.312f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {108.0f, 64.0f, 184.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				// Mailbox wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({200.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.262f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {128.0f, 64.0f, 84.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Bike room inside left wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({72.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.812f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-60.0f, 64.0f, 48.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(180.0_deg),
					}
				);

				// Bike room inside back wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({136.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.625f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-96.0f, 64.0f, 116.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Bike room inside right wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({72.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.75f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-60.0f, 64.0f, 184.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				// Bike room inside entrance left wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({28.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.58f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-24.0f, 64.0f, 170.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Bike room inside entrance right wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({52.0f, 128.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.295f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-24.0f, 64.0f, 74.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Hall entrance door wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({56.0f, 16.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.71f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {64.0f, 120.0f, -16.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(180.0_deg),
					}
				);

				// Bike room entrance door wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({56.0f, 16.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.36f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-16.0f, 120.0f, 128.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Main entrance door wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({68.0f, 16.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.53f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {54.0f, 120.0f, 184.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				// Bike room inside entrance door wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({56.0f, 16.0f}),
						.material = plasterWallMaterial,
						.textureFiltering = {
							.translation = {0.797f, 0.0f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-24.0f, 120.0f, 128.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				/// Door frames

				// Bike room entrance door frame left
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({4.0f, 108.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.875f, 0.155f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-16.0f, 54.0f, 102.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Bike room entrance door frame right
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({4.0f, 108.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.0625f, 0.155f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-16.0f, 54.0f, 154.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Bike room entrance door frame top
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({56.0f, 4.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {-0.125f, 0.125f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-16.0f, 110.0f, 128.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Bike room entrance door frame left inside
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({8.0f, 108.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.195f, 0.1575f},
							.rotation = 0.0f,
							.scale = {m1 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-20.0f, 54.0f, 104.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(180.0_deg),
					}
				);

				// Bike room entrance door frame right inside
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({8.0f, 108.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.0625f, 0.1575f},
							.rotation = 0.0f,
							.scale = {m1 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-20.0f, 54.0f, 152.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				// Bike room entrance door frame top inside
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({8.0f, 48.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.19f, -0.017f},
							.rotation = 1.0f,
							.scale = {m1 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-20.0f, 108.0f, 128.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Bike room inside entrance door frame left
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({4.0f, 108.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.0f, 0.155f},
							.rotation = 0.0f,
							.scale = {m1 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-24.0f, 54.0f, 154.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Bike room inside entrance door frame right
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({4.0f, 108.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.8125f, 0.155f},
							.rotation = 0.0f,
							.scale = {m1 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-24.0f, 54.0f, 102.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Bike room inside entrance door frame top
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({56.0f, 4.0f}),
						.material = woodDoorMaterial,
						.textureFiltering = {
							.translation = {0.0f, 0.125f},
							.rotation = 0.0f,
							.scale = {m1 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-24.0f, 110.0f, 128.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				/// Ceiling

				// Hall entrance top wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({144.0f, 112.0f}),
						.material = plasterCeilingMaterial,
						.textureFiltering = {
							.translation = {0.0f, 0.125f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {56.0f, 184.0f, -16.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(180.0_deg),
					}
				);

				// Bike room entrance top wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({200.0f, 112.0f}),
						.material = plasterCeilingMaterial,
						.textureFiltering = {
							.translation = {0.125f, 0.125f},
							.rotation = 0.0f,
							.scale = {m2 * 0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-16.0f, 184.0f, 84.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Main entrance top wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({144.0f, 112.0f}),
						.material = plasterCeilingMaterial,
						.textureFiltering = {
							.translation = {0.125f, 0.125f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {56.0f, 184.0f, 184.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				// Mailbox top wall
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({200.0f, 112.0f}),
						.material = plasterCeilingMaterial,
						.textureFiltering = {
							.translation = {0.686f, 0.125f},
							.rotation = 0.0f,
							.scale = {m2 * -0.25f, m2 * 0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {128.0f, 184.0f, 84.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				// Hall ceiling
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({144.0f, 200.0f}),
						.material = plasterCeilingMaterial,
						.textureFiltering = {
							.translation = {0.498f, 0.875f},
							.rotation = 0.5f,
							.scale = {m2 * 0.25f, m2 * -0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {56.0f, 240.0f, 84.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				// Bike room ceiling
				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({72.0f, 136.0f}),
						.material = plasterCeilingMaterial,
						.textureFiltering = {
							.translation = {0.69f, 0.258f},
							.rotation = 0.5f,
							.scale = {m2 * 0.25f, m2 * -0.25f},
						},
						.lightmap = {
							.luxelScale = {0.5f, 0.5f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {-60.0f, 128.0f, 116.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(90.0_deg),
					}
				);

				return builder.build();
			}

		public:
			explicit HallScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}