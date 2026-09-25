// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/LinearMapBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class FaceTestScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_face_test";
			}

			SceneBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				LinearMapBuilder builder({
					.flags = {},
					.skyColor = {0.6f, 0.7f, 0.9f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {100.0f, 0.0f, 32.0f},
						.direction = svec3<float32>(normalized(vec3<float32>{radians(-40.0_deg), 0.0f, radians(50.0_deg)})),
					},
				});

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

				/* const FaceVertex pentagonVertices[] = {
					{{-32.0f,  32.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 0.0f,   32.0f,  112.0f}, {0.0f, 0.0f}},
					{{ 32.0f,  32.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 16.0f,  32.0f,  64.0f }, {0.0f, 0.0f}},
					{{-16.0f,  32.0f,  64.0f }, {0.0f, 0.0f}},
				};

				Face& pentagon = builder.createFace(pentagonVertices, {0.0f, 1.0f, 0.0f});

				builder.extrudeFace(pentagon, 32.0f);

				const FaceVertex squareVertices[] = {
					{{ 64.0f,  48.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 96.0f,  48.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 96.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 64.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
				};

				Face& square = builder.createFace(squareVertices, {0.0f, 0.0f, -1.0f});

				builder.extrudeFace(square, -32.0f);

				const FaceVertex roomVertices[] = {
					{{-64.0f,  128.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 64.0f,  128.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 64.0f,  128.0f, -128.0f}, {0.0f, 0.0f}},
					{{-64.0f,  128.0f, -128.0f}, {0.0f, 0.0f}},
				};

				Face& room = builder.createFace(roomVertices, {0.0f, 1.0f, 0.0f});

				builder.extrudeFace(room, -128.0f);

				const FaceVertex concaveFaceVertices[] = {
					{{-48.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  112.0f}, {0.0f, 0.0f}},
					{{ 16.0f,  24.0f,  112.0f}, {0.0f, 0.0f}},
					{{ 16.0f,  24.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  64.0f }, {0.0f, 0.0f}},
					{{-48.0f,  24.0f,  64.0f }, {0.0f, 0.0f}},
				};

				Face& concaveFace = builder.createFace(concaveFaceVertices, {0.0f, 1.0f, 0.0f}); */

				const FaceVertex concaveFace2Vertices[] = {
					{{ 62.0f,   536.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 334.0f,  787.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 577.0f,  826.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 746.0f,  648.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 625.0f,  429.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 348.0f,  421.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 504.0f,  129.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 58.0f,   183.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 384.0f,  249.0f,  128.0f}, {0.0f, 0.0f}},
				};

				builder.createFace({
					.vertices = concaveFace2Vertices,
					.normal = {0.0f, 0.0f, -1.0f},
					.materialReference = material,
					.lightmap = {
						.luxelScale = {4.0f, 4.0f},
						.emissiveColor = {0.0f, 0.0f, 0.0f},
						.emissiveIntensity = 0.0f,
						.albedo = {1.0f, 0.2f, 0.0f},
					},
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}