// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class DDGITestScene3D final : public Scene3DProcess {
		private:
			static void createRoom(MapBuilder& builder, svec3<float32> origin, MaterialReference material, float32 lightCeilingPercent, bool open) {
				static constexpr svec2<float32> LUXEL_SCALE = {4.0f, 4.0f};
				static constexpr svec3<float32> LIGHT_EMISSIVE_COLOR = {4.0f, 4.0f, 4.0f};
				static constexpr svec3<float32> ROOM_COLOR = {1.0f, 1.0f, 1.0f};
				static constexpr float32 LIGHT_RIGHT_OFFSET_PERCENT = 0.95f;
				static constexpr TextureFilteringDescriptor TEXTURE_FILTERING = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				};

				const float32 lightLeftOffset = 128.0f * (LIGHT_RIGHT_OFFSET_PERCENT - lightCeilingPercent) - 64.0f;
				const float32 lightRightOffset = 128.0f * LIGHT_RIGHT_OFFSET_PERCENT - 64.0f;

				const svec3<float32> ceilingVertices[] = {
					origin + svec3<float32>{-64.0f,  64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f,  64.0f,  64.0f},
					origin + svec3<float32>{ 64.0f,  64.0f, -64.0f},
					origin + svec3<float32>{-64.0f,  64.0f, -64.0f},
				};
				const svec3<float32> ceilingLeftVertices[] = {
					origin + svec3<float32>{-64.0f,           64.0f, -64.0f},
					origin + svec3<float32>{lightLeftOffset,  64.0f, -64.0f},
					origin + svec3<float32>{lightLeftOffset,  64.0f,  64.0f},
					origin + svec3<float32>{-64.0f,           64.0f,  64.0f},
				};
				const svec3<float32> ceilingRightVertices[] = {
					origin + svec3<float32>{lightRightOffset,  64.0f, -64.0f},
					origin + svec3<float32>{ 64.0f,            64.0f, -64.0f},
					origin + svec3<float32>{ 64.0f,            64.0f,  64.0f},
					origin + svec3<float32>{lightRightOffset,  64.0f,  64.0f},
				};

				Face& floorFace = builder.createFace({
					.vertices = ceilingVertices,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});

				builder.extrudeFace(floorFace, -128.0f);

				builder.createFace({
					.vertices = ceilingLeftVertices,
					.normal = {0.0f, -1.0f, 0.0f},
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});
				builder.createFace({
					.vertices = ceilingRightVertices,
					.normal = {0.0f, -1.0f, 0.0f},
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});

				if (!open) {
					const svec3<float32> lightVertices[] = {
						origin + svec3<float32>{lightLeftOffset,   64.0f, -64.0f},
						origin + svec3<float32>{lightRightOffset,  64.0f, -64.0f},
						origin + svec3<float32>{lightRightOffset,  64.0f,  64.0f},
						origin + svec3<float32>{lightLeftOffset,   64.0f,  64.0f},
					};

					builder.createFace({
						.vertices = lightVertices,
						.normal = {0.0f, -1.0f, 0.0f},
						.material = material,
						.textureFiltering = TEXTURE_FILTERING,
						.lightmap = {
							.luxelScale = LUXEL_SCALE,
							.emissiveColor = LIGHT_EMISSIVE_COLOR,
							.emissiveIntensity = 1.0f,
							.albedo = ROOM_COLOR,
						},
					});
				}
			}

		protected:
			view<char8> getAssetName() const final override {
				return "3d_ddgi_test";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MapBuilder builder({
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

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

				createRoom(builder, {-192.0f,  64.0f,  384.0f}, material, 0.05f, false);
				createRoom(builder, { 0.0f,    64.0f,  384.0f}, material, 0.1f, false);
				createRoom(builder, { 192.0f,  64.0f,  384.0f}, material, 0.2f, false);

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}