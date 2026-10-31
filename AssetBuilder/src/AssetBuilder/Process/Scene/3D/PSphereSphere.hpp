// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class PSphereSphereScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "p_spheresphere";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MapBuilder builder({
					.skyColor = {0.6f, 0.7f, 0.9f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, -64.0f, -6.0f},
						.viewDirection = {0.0f, 0.0f, 1.0f},
					},
				});

				const ref<Geometry3DAsset> sphereGeometryAsset = assetLibrary->getAsset<Geometry3DAsset>("3d_unit_sphere");
				const uint32 sphereGeometry = builder.loadGeometry(sphereGeometryAsset);

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				builder.createRigidBody({
					.geometryAsset = sphereGeometryAsset,
					.geometryIndex = sphereGeometry,
					.material = material,
					.position = {0.0f, -2.0f, 0.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 4.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_SPHERE,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});
				builder.createRigidBody({
					.geometryAsset = sphereGeometryAsset,
					.geometryIndex = sphereGeometry,
					.material = material,
					.position = {0.2f, 1.0f, -0.2f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 1.0f,
					.mass = 3.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_SPHERE,
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