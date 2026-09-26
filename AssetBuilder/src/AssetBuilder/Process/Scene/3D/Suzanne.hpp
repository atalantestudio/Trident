// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class SuzanneScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_suzanne";
			}

			SceneBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr svec3<float32> FLOOR_VERTICES[] = {
					{-64.0f,  0.0f,  64.0f},
					{ 64.0f,  0.0f,  64.0f},
					{ 64.0f,  0.0f, -64.0f},
					{-64.0f,  0.0f, -64.0f},
				};
				static constexpr svec3<float32> BACK_WALL_VERTICES[] = {
					{-64.0f,  64.0f,  64.0f},
					{ 64.0f,  64.0f,  64.0f},
					{ 64.0f,  0.0f,   64.0f},
					{-64.0f,  0.0f,   64.0f},
				};
				static constexpr svec3<float32> RIGHT_WALL_VERTICES[] = {
					{ 64.0f,  64.0f,  64.0f},
					{ 64.0f,  64.0f, -64.0f},
					{ 64.0f,  0.0f,  -64.0f},
					{ 64.0f,  0.0f,   64.0f},
				};
				static constexpr TextureFilteringDescriptor TEXTURE_FILTERING = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				};

				SceneBuilder builder({
					.flags = {},
					.skyColor = {0.6f, 0.7f, 0.9f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, -32.0f, -128.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 suzanneGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("suzanne"));
				const uint32 boxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));

				const MaterialReference lightmappedDefaultMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));
				const MaterialReference debugMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				builder.createFace({
					.vertices = FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = lightmappedDefaultMaterial,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});
				builder.createFace({
					.vertices = BACK_WALL_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = lightmappedDefaultMaterial,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});
				builder.createFace({
					.vertices = RIGHT_WALL_VERTICES,
					.normal = {-1.0f, 0.0f, 0.0f},
					.material = lightmappedDefaultMaterial,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});

				builder.placeStaticProp({
					.geometryIndex = suzanneGeometry,
					.material = lightmappedDefaultMaterial,
					.transform = {
						.translation = {0.0f, 32.0f, 0.0f},
						.rotation = {0.0_rad, radians(180.0_deg), 0.0_rad},
						.scale = 12.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
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

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}