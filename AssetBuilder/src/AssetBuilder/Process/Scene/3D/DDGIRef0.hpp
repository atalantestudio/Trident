// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class DDGIRef0Scene3D final : public Scene3DProcess {
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

		protected:
			view<char8> getAssetName() const final override {
				return "3d_ddgi_ref_0";
			}

			SceneBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr svec3<float32> origin = {0.0f, 64.0f, 164.0f};
				static constexpr svec2<float32> luxelScale = {4.0f, 4.0f};
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

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("ddgi"));

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

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}