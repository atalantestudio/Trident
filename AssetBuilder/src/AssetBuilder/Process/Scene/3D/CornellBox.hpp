// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class CornellBoxScene3D final : public Scene3DProcess {
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

			static BuildStaticGeometry3D createFloorGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-549.6f,  0.0f,  559.2f},
					{ 0.0f,    0.0f,  559.2f},
					{-552.8f,  0.0f,  0.0f  },
					{ 0.0f,    0.0f,  0.0f  },
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createLightGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-343.0f,  548.8f - 0.01f,  227.0f},
					{-213.0f,  548.8f - 0.01f,  227.0f},
					{-343.0f,  548.8f - 0.01f,  332.0f},
					{-213.0f,  548.8f - 0.01f,  332.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createCeilingGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-556.0f,  548.8f,  0.0f  },
					{ 0.0f,    548.8f,  0.0f  },
					{-556.0f,  548.8f,  559.2f},
					{ 0.0f,    548.8f,  559.2f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createBackWallGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-556.0f,  548.8f,  559.2f},
					{ 0.0f,    548.8f,  559.2f},
					{-549.6f,  0.0f,    559.2f},
					{ 0.0f,    0.0f,    559.2f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createLeftWallGeometry() {
				static constexpr svec3<float32> positions[] = {
					{ 0.0f,  548.8f,  559.2f},
					{ 0.0f,  548.8f,  0.0f  },
					{ 0.0f,  0.0f,    559.2f},
					{ 0.0f,  0.0f,    0.0f  },
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createRightWallGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-556.0f,  548.8f,  0.0f  },
					{-556.0f,  548.8f,  559.2f},
					{-552.8f,  0.0f,    0.0f  },
					{-549.6f,  0.0f,    559.2f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createShortBlockFrontGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-290.0f,  165.0f,  114.0f},
					{-130.0f,  165.0f,  64.0f},
					{-290.0f,  0.0f,    114.0f},
					{-130.0f,  0.0f,    64.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createShortBlockBackGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-82.0f,   165.0f,  225.0f},
					{-240.0f,  165.0f,  272.0f},
					{-82.0f,   0.0f,    225.0f},
					{-240.0f,  0.0f,    272.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createShortBlockLeftGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-240.0f,  165.0f,  272.0f},
					{-290.0f,  165.0f,  114.0f},
					{-240.0f,  0.0f,    272.0f},
					{-290.0f,  0.0f,    114.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createShortBlockRightGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-130.0f,  165.0f,  65.0f},
					{-82.0f,  165.0f,  225.0f},
					{-130.0f,  0.0f,    65.0f},
					{-82.0f,  0.0f,    225.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createShortBlockTopGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-240.0f,  165.0f,  272.0f},
					{-82.0f,   165.0f,  225.0f},
					{-290.0f,  165.0f,  114.0f},
					{-130.0f,  165.0f,  65.0f },
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createTallBlockFrontGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-423.0f,  330.0f,  247.0f},
					{-265.0f,  330.0f,  296.0f},
					{-423.0f,  0.0f,    247.0f},
					{-265.0f,  0.0f,    296.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createTallBlockBackGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-314.0f,  330.0f,  456.0f},
					{-472.0f,  330.0f,  406.0f},
					{-314.0f,  0.0f,    456.0f},
					{-472.0f,  0.0f,    406.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createTallBlockLeftGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-472.0f,  330.0f,  406.0f},
					{-423.0f,  330.0f,  247.0f},
					{-472.0f,  0.0f,    406.0f},
					{-423.0f,  0.0f,    247.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createTallBlockRightGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-265.0f,  330.0f,  296.0f},
					{-314.0f,  330.0f,  456.0f},
					{-265.0f,  0.0f,    296.0f},
					{-314.0f,  0.0f,    456.0f},
				};

				return createGeometry(positions);
			}

			static BuildStaticGeometry3D createTallBlockTopGeometry() {
				static constexpr svec3<float32> positions[] = {
					{-472.0f,  330.0f,  406.0f},
					{-314.0f,  330.0f,  456.0f},
					{-423.0f,  330.0f,  247.0f},
					{-265.0f,  330.0f,  296.0f},
				};

				return createGeometry(positions);
			}

		protected:
			view<char8> getAssetName() const final override {
				return "3d_cornell_box";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr TextureFilteringDescriptor textureFiltering = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				};

				static const TransformAngleAxis3D transform = {
					.translation = {0.0f, 0.0f, 0.0f},
					.axis = {0.0f, 1.0f, 0.0f},
					.angle = 0.0_rad,
				};

				SceneBuilder builder({
					// TODO(wip)
					// .skyColor = {0.0f, 0.0f, 0.0f},
					.skyColor = {1.0f, 1.0f, 1.0f},

					.sunDirection = {0.0f, 0.0f, 0.0f},
					.sunColor = {0.0f, 0.0f, 0.0f},
					.sunIntensity = 0.0f,
				});

				const MaterialReference staticMaterial = builder.loadStaticMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

				const svec2<float32> luxelScale = {4.0f, 4.0f};
				const svec3<float32> greenAlbedo = {0.14f, 0.45f, 0.091f};
				const svec3<float32> redAlbedo = {0.63f, 0.065f, 0.05f};
				const svec3<float32> whiteAlbedo = {0.725f, 0.71f, 0.68f};
				const svec3<float32> lightEmissiveColor = {17.0f, 12.0f, 4.0f};

				builder.placeSurface(
					{
						.geometry = createFloorGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createLightGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.emissiveColor = lightEmissiveColor,
							.emissiveIntensity = 1.0f,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createCeilingGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createBackWallGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createLeftWallGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = redAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createRightWallGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = greenAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createShortBlockFrontGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createShortBlockBackGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createShortBlockLeftGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createShortBlockRightGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createShortBlockTopGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createTallBlockFrontGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createTallBlockBackGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createTallBlockLeftGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createTallBlockRightGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				builder.placeSurface(
					{
						.geometry = createTallBlockTopGeometry(),
						.material = staticMaterial,
						.textureFiltering = textureFiltering,
						.lightmap = {
							.luxelScale = luxelScale,
							.albedo = whiteAlbedo,
						},
					},
					transform
				);

				return builder.build();
			}

		public:
			explicit CornellBoxScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}