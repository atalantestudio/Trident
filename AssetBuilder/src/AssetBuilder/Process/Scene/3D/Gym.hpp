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
				static constexpr vec3<float32> FLOOR_VERTICES[] = {
					{-128.0f,  0.0f,  128.0f},
					{ 128.0f,  0.0f,  128.0f},
					{ 128.0f,  0.0f, -128.0f},
					// {-128.0f,  0.0f, -128.0f},
				};
				static constexpr vec3<float32> WALL_VERTICES[] = {
					{-128.0f,  128.0f,  128.0f},
					{ 128.0f,  128.0f,  128.0f},
					{-128.0f,  -128.0f, 128.0f},
					// { 128.0f,  -128.0f, 128.0f},
				};

				// Texture scale multipliers
				static constexpr float32 m1 = 8.0f;

				MapBuilder builder({
					.skyColor = {1.0f, 1.0f, 1.0f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, -63.0f, -5.0f},
						.viewDirection = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 rectangularCuboidGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_rectangular_cuboid"));
				const uint32 playerHullGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("playerhull"));

				const MaterialReference floorMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));
				const MaterialReference playerMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("pbr_default"));

				builder.createFace({
					.vertices = FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = floorMaterial,
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
				/* builder.createFace({
					.vertices = WALL_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = floorMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {m1 * 0.25f, m1 * 0.25f},
					},
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				}); */

				/* builder.createRigidBody({
					.geometryIndex = playerHullGeometry,
					.material = playerMaterial,
					.position = {0.0f, -1.0f, 0.2f},
					// .rotation = {0.0f, 0.0f, 0.15f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {5.0f, 0.1f, 1.0f},
					.friction = 0.0f,
					.restitution = 0.0f,
				}); */

				builder.createRigidBody({
					.geometryIndex = playerHullGeometry,
					.material = playerMaterial,
					.position = { 0.0f, 64.0f,  0.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
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