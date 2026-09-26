// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class SlidingScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_sliding";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MapBuilder builder({
					.skyColor = {0.6f, 0.7f, 0.9f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, -62.0f, -6.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 unitBoxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));
				const uint32 rectangularCuboidGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_rectangular_cuboid"));

				const MaterialReference debugMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				// Flat floor
				/* builder.createRigidBody({
					.geometryIndex = rectangularCuboidGeometry,
					.material = debugMaterial,
					.position = {0.0_m, 0.0_m, 0.0_m},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				}); */

				// Inclined floor
				builder.createRigidBody({
					.geometryIndex = rectangularCuboidGeometry,
					.material = debugMaterial,
					.position = {0.0f, 0.0f, 0.2f},
					.rotation = {0.0f, 0.0f, 0.15f},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {5.0f, 0.1f, 1.0f},
					.friction = 0.0f,
					.restitution = 0.0f,
				});

				// Box 2
				builder.createRigidBody({
					.geometryIndex = unitBoxGeometry,
					.material = debugMaterial,
					.position = {0.0_m, 2.2_m, 0.0_m},
					.rotation = {0.0f, pi<float32>() * 0.25f, pi<float32>() * 0.25f},
					.uniformScale = 1.0f,
					.mass = 3.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.0f,
					.restitution = 0.0f,
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}