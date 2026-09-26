// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class DebugPlayerMovementMap final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_debug_player_movement";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr svec3<float32> FLOOR_VERTICES[] = {
					{-256.0f,  0.0f,  256.0f},
					{ 256.0f,  0.0f,  256.0f},
					{ 256.0f,  0.0f, -256.0f},
					{-256.0f,  0.0f, -256.0f},
				};
				static constexpr svec3<float32> CEILING_VERTICES[] = {
					{-256.0f,  128.0f,  256.0f},
					{ 256.0f,  128.0f,  256.0f},
					{ 256.0f,  128.0f, -256.0f},
					{-256.0f,  128.0f, -256.0f},
				};

				// Texture scale multipliers
				static constexpr float32 m1 = 128.0f;
				static constexpr float32 m2 = 512.0f;

				MapBuilder builder({
					.skyColor = {0.0f, 0.0f, 0.0f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 boxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));

				const MaterialReference lightmappedGrayMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_debug_gray"));
				const MaterialReference lightmappedWallMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_debug_wall"));
				const MaterialReference debugMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				Face& ceilingFace = builder.createFace({
					.vertices = CEILING_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = lightmappedWallMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m2 * 0.25f, m2 * 0.25f},
					},
					.lightmap = {
						.luxelScale = {4.0f, 4.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});

				builder.extrudeFace(ceilingFace, -256.0f);

				builder.createFace({
					.vertices = FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = lightmappedGrayMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 * 0.25f, m1 * 0.25f},
					},
					.lightmap = {
						.luxelScale = {4.0f, 4.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});

				/* // Box
				builder.createRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {1.0_m, 0.5_m, 3.0_m},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Player
				builder.createRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {0.0_m, 2.0_m, 0.0_m},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = 70.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {0.7f, 1.8f, 0.7f},
					.friction = 0.2f,
					.restitution = 0.0f,
				}); */

				/* // Box
				builder.createRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {128.0f, 8.0f, 128.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = 20.0_kg,
					// .mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {64.0f, 16.0f, 64.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Player
				builder.createRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {0.0f, 128.0f, 0.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = 70.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {33.0f, 77.0f, 33.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				}); */

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}