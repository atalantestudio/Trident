// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	// This scene was created with the official Cornell Box measurements.
	// See https://bowers.cornell.edu/computer-graphics/data for more info.
	class CornellBoxScene3D final : public Scene3DProcess {
		private:
			static svec3<float32> calculateNormal(view<svec3<float32>> vertices) {
				const vec3<float32> ab = vertices[1] - vertices[0];
				const vec3<float32> ac = vertices[2] - vertices[0];

				return svec3<float32>(normalized(cross(ab, ac)));
			}

		protected:
			view<char8> getAssetName() const final override {
				return "3d_cornell_box";
			}

			SceneBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr float32 LIGHT_BIAS = 0.01f;
				static constexpr float32 SHORT_BLOCK_HEIGHT = 165.0f;
				static constexpr float32 TALL_BLOCK_HEIGHT = 330.0f;
				static constexpr svec2<float32> LUXEL_SCALE = {4.0f, 4.0f};
				static constexpr svec3<float32> LIGHT_EMISSIVE_COLOR = {17.0f, 12.0f, 4.0f};
				static constexpr svec3<float32> LEFT_WALL_COLOR = {0.63f, 0.065f, 0.05f};
				static constexpr svec3<float32> RIGHT_WALL_COLOR = {0.14f, 0.45f, 0.091f};
				static constexpr svec3<float32> ROOM_COLOR = {0.725f, 0.71f, 0.68f};
				static constexpr svec3<float32> VERTICES[] = {
					{-549.6f,  0.0f,  559.2f},
					{ 0.0f,    0.0f,  559.2f},
					{ 0.0f,    0.0f,  0.0f  },
					{-552.8f,  0.0f,  0.0f  },
					{-343.0f,  548.8f - LIGHT_BIAS,  227.0f},
					{-213.0f,  548.8f - LIGHT_BIAS,  227.0f},
					{-213.0f,  548.8f - LIGHT_BIAS,  332.0f},
					{-343.0f,  548.8f - LIGHT_BIAS,  332.0f},
					{-556.0f,  548.8f,  0.0f  },
					{ 0.0f,    548.8f,  0.0f  },
					{ 0.0f,    548.8f,  559.2f},
					{-556.0f,  548.8f,  559.2f},
					{-556.0f,  548.8f,  559.2f},
					{ 0.0f,    548.8f,  559.2f},
					{ 0.0f,    0.0f,    559.2f},
					{-549.6f,  0.0f,    559.2f},
					{-556.0f,  548.8f,  0.0f  },
					{-556.0f,  548.8f,  559.2f},
					{-549.6f,  0.0f,    559.2f},
					{-552.8f,  0.0f,    0.0f  },
					{ 0.0f,  548.8f,  559.2f},
					{ 0.0f,  548.8f,  0.0f  },
					{ 0.0f,  0.0f,    0.0f  },
					{ 0.0f,  0.0f,    559.2f},
					{-240.0f,  0.0f,  272.0f},
					{-82.0f,   0.0f,  225.0f},
					{-130.0f,  0.0f,  65.0f },
					{-290.0f,  0.0f,  114.0f},
					{-472.0f,  0.0f,  406.0f},
					{-314.0f,  0.0f,  456.0f},
					{-265.0f,  0.0f,  296.0f},
					{-423.0f,  0.0f,  247.0f},
				};
				static constexpr view<svec3<float32>> FLOOR_VERTICES(&VERTICES[0], 4);
				static constexpr view<svec3<float32>> LIGHT_VERTICES(&VERTICES[4], 4);
				static constexpr view<svec3<float32>> CEILING_VERTICES(&VERTICES[8], 4);
				static constexpr view<svec3<float32>> BACK_WALL_VERTICES(&VERTICES[12], 4);
				static constexpr view<svec3<float32>> LEFT_WALL_VERTICES(&VERTICES[16], 4);
				static constexpr view<svec3<float32>> RIGHT_WALL_VERTICES(&VERTICES[20], 4);
				static constexpr view<svec3<float32>> SHORT_BLOCK_BOTTOM_VERTICES(&VERTICES[24], 4);
				static constexpr view<svec3<float32>> TALL_BLOCK_BOTTOM_VERTICES(&VERTICES[28], 4);
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
						.position = {-278.0f, 273.0f, -800.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

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
					.vertices = LIGHT_VERTICES,
					.normal = calculateNormal(LIGHT_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.emissiveColor = LIGHT_EMISSIVE_COLOR,
						.emissiveIntensity = 1.0f,
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
						.albedo = LEFT_WALL_COLOR,
					},
				});
				builder.createFace({
					.vertices = RIGHT_WALL_VERTICES,
					.normal = calculateNormal(RIGHT_WALL_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = RIGHT_WALL_COLOR,
					},
				});

				Face& shortBlockFace = builder.createFace({
					.vertices = SHORT_BLOCK_BOTTOM_VERTICES,
					.normal = calculateNormal(SHORT_BLOCK_BOTTOM_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});

				builder.extrudeFace(shortBlockFace, SHORT_BLOCK_HEIGHT);

				Face& tallBlockFace = builder.createFace({
					.vertices = TALL_BLOCK_BOTTOM_VERTICES,
					.normal = calculateNormal(TALL_BLOCK_BOTTOM_VERTICES),
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = LUXEL_SCALE,
						.albedo = ROOM_COLOR,
					},
				});

				builder.extrudeFace(tallBlockFace, TALL_BLOCK_HEIGHT);

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}