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
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});

				const uint32 boxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box"));
				const uint32 boxGeometry2 = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));

				const MaterialReference lightmappedDefaultMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));
				const MaterialReference debugMaterial = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({144.0f, 256.0f}),
						.material = lightmappedDefaultMaterial,
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.lightmap = {
							.luxelScale = {1.0f, 1.0f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {0.0f, 0.0f, 48.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				builder.placeStaticProp({
					.geometryIndex = boxGeometry,
					.material = lightmappedDefaultMaterial,
					.transform = {
						.translation = {-32.0f, 16.0f, 128.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 32.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.tintColor = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {32.0f, 32.0f},
				});

				/* builder.placeStaticProp({
					.geometryIndex = boxGeometry,
					.material = lightmappedDefaultMaterial,
					.transform = {
						.translation = {32.0f, 16.0f, 128.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 32.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.tintColor = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {32.0f, 32.0f},
				}); */

				builder.placeRigidBody({
					.geometryIndex = boxGeometry2,
					.material = debugMaterial,
					.position = {32.0f, 16.0f, 128.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 32.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.0f,
					.restitution = 0.0f,
				});

				return builder.build();
			}

		public:
			explicit CubesScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}