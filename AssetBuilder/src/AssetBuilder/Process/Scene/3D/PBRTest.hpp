// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class PBRTestScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_pbr_test";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.skyColor = {0.0f, 0.0f, 0.0f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 0.0f,
				});

				const uint32 geometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_sphere"));

				const MaterialReference material = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("pbr_debug_default"));

				static constexpr sint32 columnCount = 7;
				static constexpr sint32 rowCount = 7;

				sint32 columnIndex = 0;

				while (columnIndex < columnCount) {
					sint32 rowIndex = 0;

					while (rowIndex < rowCount) {
						builder.placeRigidBody({
							.geometryIndex = geometry,
							.material = material,
							.position = {(columnIndex - (columnCount / 2)) * 32.0f, (rowIndex - (rowCount / 2)) * 32.0f + 64.0f, 128.0f},
							.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
							.uniformScale = 26.0f,
							.mass = max<float32>(),
							.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
							.inertiaScale = {1.0f, 1.0f, 1.0f},
							.friction = 0.0f,
							.restitution = 0.0f,
						});

						rowIndex += 1;
					}

					columnIndex += 1;
				}

				static constexpr uint32 pointLightCount = 4;
				static const svec3<float32> pointLightPositions[] = {
					{-256.0f,  320.0f, 0.0f},
					{ 256.0f,  320.0f, 0.0f},
					{-256.0f, -192.0f, 0.0f},
					{ 256.0f, -192.0f, 0.0f},
				};

				uint32 pointLightIndex = 0;

				while (pointLightIndex < pointLightCount) {
					builder.placePointLight({
						.color = {1.0f, 1.0f, 1.0f},
						.intensity = 30000.0f,
						.position = pointLightPositions[pointLightIndex],
					});

					pointLightIndex += 1;
				}

				return builder.build();
			}

		public:
			explicit PBRTestScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}