// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class EdgeContactScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_edge_contact";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = Scene3DBuildFlags::LIGHTMAP_SUN_CONTRIBUTION,
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});

				const uint32 unitBoxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));

				const MaterialReference debugMaterial = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				// Box 1
				builder.placeRigidBody({
					.geometryIndex = unitBoxGeometry,
					.material = debugMaterial,
					.position = {0.0_m, 0.0_m, 0.0_m},
					.rotation = {0.0f, 0.0f, 0.0f},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Box 2
				builder.placeRigidBody({
					.geometryIndex = unitBoxGeometry,
					.material = debugMaterial,
					.position = {1.0_m, 2.0_m, 0.0_m},
					.rotation = {0.0f, -0.3f, /* -pi<float32>() / 4.0f */ -0.4f},
					.uniformScale = 1.0f,
					.mass = 3.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.0f,
					.restitution = 0.0f,
				});

				return builder.build();
			}

		public:
			explicit EdgeContactScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}