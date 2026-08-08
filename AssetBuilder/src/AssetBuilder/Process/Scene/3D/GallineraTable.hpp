// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class GallineraTableScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_gallinera_table";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});

				const uint32 gallineraTableGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("gallinera_table"));
				const uint32 boxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));

				const MaterialReference lightmappedDefaultMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));
				const MaterialReference gallineraTableMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("lpbr_gallinera_table_1k"));
				const MaterialReference debugMaterial = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({128.0f, 128.0f}),
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
						.translation = {0.0f, 0.0f, 0.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({128.0f, 64.0f}),
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
						.translation = {0.0f, 32.0f, 64.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({128.0f, 64.0f}),
						.material = gallineraTableMaterial,
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
						.translation = {64.0f, 32.0f, 0.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				builder.placeStaticProp({
					.geometryIndex = gallineraTableGeometry,
					.material = gallineraTableMaterial,
					.transform = {
						.translation = {0.0f, 0.0f, 0.0f},
						.rotation = {radians(90.0_deg), 0.0_rad, 0.0_rad},
						.scale = 64.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.tintColor = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {512.0f, 512.0f},
				});

				builder.placeRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {0.0f, 32.0f, -128.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 33.0f,
					.mass = 70.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {33.0f, 77.0f, 33.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				return builder.build();
			}

		public:
			explicit GallineraTableScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}