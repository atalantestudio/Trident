// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/StaticGeometry3D.hpp"
#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/Hull.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class BrushTestSceneProcess final : public Scene3DProcess {
		private:
			static StaticGeometry3D createBrushGeometry(svec2<float32> scale) {
				return {
					.vertexComponents = {
						-0.5f * scale.x,  0.5f * scale.y,  0.0f,
						 0.5f * scale.x,  0.5f * scale.y,  0.0f,
						-0.5f * scale.x, -0.5f * scale.y,  0.0f,
						 0.5f * scale.x, -0.5f * scale.y,  0.0f,
					},
					.vertexTextureCoordinateComponents = {
						0.0f, 0.0f,
						1.0f, 0.0f,
						0.0f, 1.0f,
						1.0f, 1.0f,
					},
					.vertexIndices = {
						0, 1, 2,
						1, 3, 2,
					},
					.vertexTextureCoordinateIndices = {
						0, 1, 2,
						1, 3, 2,
					},
					.edges = {0, 1, 3, 2},
					.normal = {0.0f, 0.0f, -1.0f},
					.scale = scale,
				};
			}

		protected:
			view<char8> getAssetName() const final override {
				return "BrushTest";
			}

			Scene3DData createScene(std::filesystem::path rootOutputPath) const final override {
				StaticSceneBuilder builder(rootOutputPath);
				builder.addMaterialShader("Lightmapped.frag", ShaderStage::FRAGMENT, "main", {
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::FRAGMENT,
					},
					{
						.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
						.descriptorCount = 1,
						.shaderStages = ShaderStage::FRAGMENT,
					},
				});

				builder.addMaterial(101, 0);

				const StaticGeometry3D rectangleGeometry = createBrushGeometry({200.0f, 128.0f});

				builder.placeSurface(
					{
						.geometry = rectangleGeometry,
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f / 128.0f, 1.0f / 128.0f},
						},
						.materialIndex = 0,
						.luxelScale = {1.0f, 1.0f},
						.carvings = {
							{
								.offset = {8.0f, 8.0f},
								.size = {40.0f, 84.0f},
							},
							{
								.offset = {56.0f, 8.0f},
								.size = {40.0f, 84.0f},
							},
							{
								.offset = {104.0f, 8.0f},
								.size = {40.0f, 84.0f},
							},
							{
								.offset = {152.0f, 8.0f},
								.size = {40.0f, 84.0f},
							},
						},
					},
					{
						.position = {0.0f, 0.0f, 128.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0f,
					}
				);

				return {
					.staticScene = builder.build(),
					.entityCount = 1,
					.geometryAssetIdentifiers = {
						38,
					},
					.geometryMappings = {
						{0, {0}},
					},
					.materialMappings = {
						{0, {1}},
					},
					.rigidBodyMappings = {
						// Player
						{
							0,
							createRigidBody({
								.position = {0.0f, 0.0f, 0.0f},
								.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
								.scale = {33.0f, 77.0f, 33.0f},
								.uniformScale = 33.0f,
								.mass = 70.0_kg,
								.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},
					},
				};
			}

		public:
			explicit BrushTestSceneProcess(BuildAssetMap* assetMap) :
				Scene3DProcess(assetMap)
			{}
	};
}