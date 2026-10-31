// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class GymMap final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "gym";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr vec2<float32> LUXEL_SCALE = {4.0f, 4.0f};
				static constexpr vec3<float32> EXTRUDED_FLOOR_VERTICES[] = {
					{-2.0f,  0.0f,  2.0f},
					{ 2.0f,  0.0f,  2.0f},
					{ 2.0f,  0.0f, -2.0f},
					// {-2.0f,  0.0f, -2.0f},
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
						.position = {0.0f, -63.0f, -5.0f},
						.viewDirection = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 playerHullGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));
				const uint32 unitPlaneGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_plane"));

				const MaterialReference lightmappedGrayMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_debug_gray"));
				const MaterialReference lightmappedWallMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_debug_wall"));
				const MaterialReference debugMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				Face& floorFace = builder.createFace({
					.vertices = EXTRUDED_FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = lightmappedGrayMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 * 0.25f, m1 * 0.25f},
					},
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});

				// Player
				builder.createRigidBody({
					.geometryIndex = playerHullGeometry,
					.material = debugMaterial,

					// TODO(physics/3d/center-of-mass): Position the player at (0 0 0).
					.position = {-0.1f, 3.0f, -0.1f},

					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = 70.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}