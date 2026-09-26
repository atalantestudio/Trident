// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class DDGIRef0Scene3D final : public Scene3DProcess {
		private:
			static svec3<float32> calculateNormal(view<svec3<float32>> vertices) {
				const vec3<float32> ab = vertices[1] - vertices[0];
				const vec3<float32> ac = vertices[2] - vertices[0];

				return svec3<float32>(normalized(cross(ab, ac)));
			}

		protected:
			view<char8> getAssetName() const final override {
				return "3d_ddgi_ref_0";
			}

			SceneBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr svec2<float32> LUXEL_SCALE = {4.0f, 4.0f};
				static constexpr svec3<float32> ROOM_COLOR = {1.0f, 1.0f, 1.0f};
				static constexpr svec3<float32> VERTICES[] = {
					{-64.0f,  0.0f,    228.0f},
					{ 64.0f,  0.0f,    228.0f},
					{ 64.0f,  0.0f,    100.0f},
					{-64.0f,  0.0f,    100.0f},
					{-64.0f,  128.0f,  100.0f},
					{ 64.0f,  128.0f,  100.0f},
					{ 64.0f,  128.0f,  228.0f},
					{-64.0f,  128.0f,  228.0f},
					{ 64.0f,  128.0f,  100.0f},
					{-64.0f,  128.0f,  100.0f},
					{-64.0f,  0.0f,    100.0f},
					{ 64.0f,  0.0f,    100.0f},
					{-64.0f,  128.0f,  228.0f},
					{ 64.0f,  128.0f,  228.0f},
					{ 64.0f,  0.0f,    228.0f},
					{-64.0f,  0.0f,    228.0f},
					{-64.0f,  128.0f,  100.0f},
					{-64.0f,  128.0f,  228.0f},
					{-64.0f,  0.0f,    228.0f},
					{-64.0f,  0.0f,    100.0f},
					{ 64.0f,  128.0f,  228.0f},
					{ 64.0f,  128.0f,  100.0f},
					{ 64.0f,  0.0f,    100.0f},
					{ 64.0f,  0.0f,    228.0f},
				};
				static constexpr view<svec3<float32>> FLOOR_VERTICES(&VERTICES[0], 4);
				static constexpr view<svec3<float32>> CEILING_VERTICES(&VERTICES[4], 4);
				static constexpr view<svec3<float32>> FRONT_WALL_VERTICES(&VERTICES[8], 4);
				static constexpr view<svec3<float32>> BACK_WALL_VERTICES(&VERTICES[12], 4);
				static constexpr view<svec3<float32>> LEFT_WALL_VERTICES(&VERTICES[16], 4);
				static constexpr view<svec3<float32>> RIGHT_WALL_VERTICES(&VERTICES[20], 4);
				static constexpr TextureFilteringDescriptor TEXTURE_FILTERING = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				};

				SceneBuilder builder({
					.flags = {},
					.skyColor = {0.0f, 0.0f, 0.0f},
					.environmentMapImageAsset = nullptr,
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {0.0f, 0.0f, 0.0f},
					.sunIntensity = 0.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 boxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box"));

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("ddgi"));

				builder.createFace({
					.vertices = FLOOR_VERTICES,
					.normal = calculateNormal(FLOOR_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});
				builder.createFace({
					.vertices = CEILING_VERTICES,
					.normal = calculateNormal(CEILING_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});
				builder.createFace({
					.vertices = FRONT_WALL_VERTICES,
					.normal = calculateNormal(FRONT_WALL_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});
				builder.createFace({
					.vertices = BACK_WALL_VERTICES,
					.normal = calculateNormal(BACK_WALL_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});
				builder.createFace({
					.vertices = LEFT_WALL_VERTICES,
					.normal = calculateNormal(LEFT_WALL_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});
				builder.createFace({
					.vertices = RIGHT_WALL_VERTICES,
					.normal = calculateNormal(RIGHT_WALL_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});

				builder.placeStaticProp({
					.geometryIndex = boxGeometry,
					.material = material,
					.transform = {
						.translation = {32.0f, 32.0f, 196.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 32.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {32.0f, 32.0f},
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}