// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class CubesScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_cubes";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = {},
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});

				const uint32 boxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box"));
				const uint32 boxGeometry2 = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));

				const MaterialReference lightmappedDefaultMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));
				const MaterialReference pbrDefaultMaterial = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("pbr_default"));

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({512.0f, 128.0f}),
						.material = lightmappedDefaultMaterial,
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.lightmap = {
							.luxelScale = {1.0f, 1.0f},
							.albedo = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {0.0f, 0.0f, 128.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				static constexpr float32 positionBias = 2.37f;
				static constexpr float32 spacing = 48.0f;

				uint32 boxIndex = 0;

				while (boxIndex < 4) {
					builder.placeStaticProp({
						.geometryIndex = boxGeometry,
						.material = lightmappedDefaultMaterial,
						.transform = {
							.translation = {(static_cast<sint32>(boxIndex) - 2) * spacing + positionBias, 16.0f, 128.0f},
							.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
							.scale = 32.0f,
						},
						.lightmap = {
							.luxelScale = {1.0f, 1.0f},
							.albedo = {1.0f, 1.0f, 1.0f},
						},
						.lightmapScale = {32.0f, 32.0f},
					});

					boxIndex += 1;
				}

				builder.placeRigidBody({
					.geometryIndex = boxGeometry2,
					.material = pbrDefaultMaterial,
					.position = {(static_cast<sint32>(boxIndex) - 2) * spacing + positionBias, 16.0f, 128.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 32.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.0f,
					.restitution = 0.0f,
				});

				builder.placePointLight({
					.color = {1.0f, 1.0f, 1.0f},
					.intensity = 300.0f,
					.position = {0.0f, 256.0f, 0.0f},
				});

				return builder.build();
			}

		public:
			explicit CubesScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}