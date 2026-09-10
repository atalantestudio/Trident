// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class TestGIScene3D final : public Scene3DProcess {
		private:
			static BuildStaticGeometry3D createGeometry(view<svec3<float32>> positions) {
				const vec3<float32> ab = positions[1] - positions[0];
				const vec3<float32> ac = positions[2] - positions[0];
				const svec3<float32> normal = svec3<float32>(normalized(cross(ab, ac)));
				const svec2<float32> scale = {
					length(ab),
					length(ac),
				};

				return {
					.vertices = {
						{positions[0], normal, { 0.0f,  0.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
						{positions[1], normal, { 0.0f,  0.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
						{positions[2], normal, { 0.0f,  0.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
						{positions[3], normal, { 0.0f,  0.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
					},
					.indices = {
						0, 1, 2,
						1, 3, 2,
					},
					.edgeOriginVertexIndices = {0, 1, 3, 2},
					.normal = normal,
					.scale = scale,
				};
			}

			static BuildStaticGeometry3D createFloorGeometry(svec3<float32> origin) {
				const svec3<float32> positions[] = {
					origin + svec3<float32>{-64.0f, -64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f, -64.0f,  64.0f},
					origin + svec3<float32>{-64.0f, -64.0f, -64.0f},
					origin + svec3<float32>{ 64.0f, -64.0f, -64.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createFrontWallGeometry(svec3<float32> origin) {
				const svec3<float32> positions[] = {
					origin + svec3<float32>{-64.0f,  64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f,  64.0f,  64.0f},
					origin + svec3<float32>{-64.0f, -64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f, -64.0f,  64.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createRightWallGeometry(svec3<float32> origin) {
				const svec3<float32> positions[] = {
					origin + svec3<float32>{ 64.0f,  64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f,  64.0f, -64.0f},
					origin + svec3<float32>{ 64.0f, -64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f, -64.0f, -64.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createBackWallGeometry(svec3<float32> origin) {
				const svec3<float32> positions[] = {
					origin + svec3<float32>{ 64.0f,  64.0f, -64.0f},
					origin + svec3<float32>{-64.0f,  64.0f, -64.0f},
					origin + svec3<float32>{ 64.0f, -64.0f, -64.0f},
					origin + svec3<float32>{-64.0f, -64.0f, -64.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createLeftWallGeometry(svec3<float32> origin) {
				const svec3<float32> positions[] = {
					origin + svec3<float32>{-64.0f,  64.0f, -64.0f},
					origin + svec3<float32>{-64.0f,  64.0f,  64.0f},
					origin + svec3<float32>{-64.0f, -64.0f, -64.0f},
					origin + svec3<float32>{-64.0f, -64.0f,  64.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createCeilingGeometry(svec3<float32> origin) {
				const svec3<float32> positions[] = {
					origin + svec3<float32>{-64.0f, 64.0f, -64.0f},
					origin + svec3<float32>{ 64.0f, 64.0f, -64.0f},
					origin + svec3<float32>{-64.0f, 64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f, 64.0f,  64.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createCeilingAreaLightGeometry(svec3<float32> origin, float32 rightOffsetPercent, float32 widthPercent) {
				static constexpr float32 ceilingBias = 0.01f;

				const float32 rightOffset = 128.0f * rightOffsetPercent - 64.0f;
				const float32 leftOffset = rightOffset - widthPercent * 128.0f;

				const svec3<float32> positions[] = {
					origin + svec3<float32>{leftOffset,   64.0f - ceilingBias, -64.0f},
					origin + svec3<float32>{rightOffset,  64.0f - ceilingBias, -64.0f},
					origin + svec3<float32>{leftOffset,   64.0f - ceilingBias,  64.0f},
					origin + svec3<float32>{rightOffset,  64.0f - ceilingBias,  64.0f},
				};

				return createGeometry(positions);
			}

			static void createRoom(SceneBuilder& builder, svec3<float32> origin, float32 lightCeilingPercent, MaterialReference material) {
				static constexpr float32 lightRightOffsetPercent = 0.95f;
				static constexpr svec2<float32> luxelScale = {4.0f, 4.0f};
				static constexpr svec3<float32> emissive = {4.0f, 4.0f, 4.0f};
				static constexpr svec3<float32> white = {1.0f, 1.0f, 1.0f};
				static constexpr TextureFilteringDescriptor textureFiltering = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				};

				static const TransformAngleAxis3D transform = {
					.translation = {0.0f, 0.0f, 0.0f},
					.axis = {0.0f, 0.0f, 0.0f},
					.angle = 0.0_rad,
				};

				builder.placeSurface(
					{
						.geometry = createFloorGeometry(origin),
						.material = material,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = white,
						},
					},
					transform
				);
				builder.placeSurface(
					{
						.geometry = createFrontWallGeometry(origin),
						.material = material,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = white,
						},
					},
					transform
				);
				builder.placeSurface(
					{
						.geometry = createRightWallGeometry(origin),
						.material = material,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = white,
						},
					},
					transform
				);
				builder.placeSurface(
					{
						.geometry = createBackWallGeometry(origin),
						.material = material,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = white,
						},
					},
					transform
				);
				builder.placeSurface(
					{
						.geometry = createLeftWallGeometry(origin),
						.material = material,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = white,
						},
					},
					transform
				);
				builder.placeSurface(
					{
						.geometry = createCeilingGeometry(origin),
						.material = material,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = white,
						},
					},
					transform
				);
				builder.placeSurface(
					{
						.geometry = createCeilingAreaLightGeometry(origin, lightRightOffsetPercent, lightCeilingPercent),
						.material = material,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.emissiveColor = emissive,
							.emissiveIntensity = 1.0f,
							.albedo = white,
						},
					},
					transform
				);
			}

		protected:
			view<char8> getAssetName() const final override {
				return "3d_test_gi";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = {},
					.skyColor = {0.0f, 0.0f, 0.0f},
					.skyboxImageAsset = nullptr,
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {0.0f, 0.0f, 0.0f},
					.sunIntensity = 0.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

				createRoom(builder, {-192.0f,  64.0f,  384.0f}, 0.05f, material);
				createRoom(builder, { 0.0f,    64.0f,  384.0f}, 0.1f, material);
				createRoom(builder, { 192.0f,  64.0f,  384.0f}, 0.2f, material);

				return builder.build();
			}

		public:
			explicit TestGIScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}