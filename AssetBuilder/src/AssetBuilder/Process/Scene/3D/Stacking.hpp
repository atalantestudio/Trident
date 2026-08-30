// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class StackingScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_stacking";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = {},
					.skyColor = {0.6f, 0.7f, 0.9f},
					.skyboxImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_2k"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, -62.0f, -6.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 rectangularCuboidGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_rectangular_cuboid"));

				const MaterialReference debugMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				// Floor
				builder.placeRigidBody({
					.geometryIndex = rectangularCuboidGeometry,
					.material = debugMaterial,
					.position = {0.0f, -0.05f, 0.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {10.0f, 0.1f, 10.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Box 1
				builder.placeRigidBody({
					.geometryIndex = rectangularCuboidGeometry,
					.material = debugMaterial,
					.position = {0.0_m, 1.5_m, 0.0_m},
					.rotation = {0.2f, 0.1f, 0.0f},
					.uniformScale = 1.0f,
					.mass = 3.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Box 2
				builder.placeRigidBody({
					.geometryIndex = rectangularCuboidGeometry,
					.material = debugMaterial,
					.position = {0.1_m, 3.0_m, -0.1_m},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 1.0f,
					.mass = 3.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Box 3
				builder.placeRigidBody({
					.geometryIndex = rectangularCuboidGeometry,
					.material = debugMaterial,
					.position = {0.0_m, 4.5_m, 0.05_m},
					// .rotation = {0.0f, pi<float32>() * 0.25f, pi<float32>() * 0.25f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 1.0f,
					.mass = 3.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				return builder.build();
			}

		public:
			explicit StackingScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}