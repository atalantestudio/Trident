// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/GeometryData.hpp"
#include "../../../Subprocess/Hull.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class BrushTestSceneProcess final : public Scene3DProcess {
		private:
			static Geometry3DData createBrushGeometry(svec2<float32> scale) {
				Geometry3DData geometry = {
					.vertexPositionComponents = {
						-0.5f * scale.x,  0.5f * scale.y,  0.0f,
						 0.5f * scale.x,  0.5f * scale.y,  0.0f,
						-0.5f * scale.x, -0.5f * scale.y,  0.0f,
						 0.5f * scale.x, -0.5f * scale.y,  0.0f,
					},
					.vertexNormalComponents = {
						0.0f, 0.0f, -1.0f,
					},
					.vertexTextureCoordinateComponents = {
						0.0f, 0.0f,
						1.0f, 0.0f,
						0.0f, 1.0f,
						1.0f, 1.0f,
					},
					.triangleTangentComponents = {
						1.0f,  0.0f,  0.0f,
						1.0f,  0.0f,  0.0f,
					},
					.vertexPositionIndices = {
						0, 1, 2,
						1, 3, 2,
					},
					.vertexNormalIndices = {
						0, 0, 0,
						0, 0, 0,
					},
					.vertexTextureCoordinateIndices = {
						0, 1, 2,
						1, 3, 2,
					},
					.flags = GeometryAssetFlags::HAS_HULL,
					.hullLineIndices = {
						0, 1,
						1, 3,
						3, 2,
						2, 0,
					},
					.hullEdges = {
						{.originVertexIndex = 0, .faceIndex = 0},
						{.originVertexIndex = 1, .faceIndex = 0},
						{.originVertexIndex = 3, .faceIndex = 0},
						{.originVertexIndex = 2, .faceIndex = 0},
					},
					.hullFaceEdgeIndices = {
						0, 1, 2, 3,
					},
					.hullFaces = {
						{{0.0f, 0.0f, -1.0f}, 0.0f, {0, 4}},
					},
				};

				geometry.hull = createHull<Hull3DType::POLYGON>(geometry.vertexPositionComponents);

				return geometry;
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

				builder.addMaterial(100, 0);

				const Geometry3DData brushGeometry = createBrushGeometry({192.0f, 128.0f});

				// TODO(asset-builder/scene/surface-snapping)
				/* builder.addSurface2({
					.geometry = brushGeometry,
					.materialIndex = 0,
					.transform = {
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0f,
					},
					.textureFiltering = {
						.translation = {0.25f, 0.625f},
						.rotation = 0.0f,
						.scale = {(1.0f / 256.0f) / -0.25f, (1.0f / 256.0f) / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				}); */

				return {
					.staticScene = builder.build(),
					.entityCount = 1,
					.geometryAssetIdentifiers = {
						// TODO(debug-only): UnitBox
						38,
					},
					.geometryMappings = {
						{0, {0}},
					},
					.materialMappings = {
						// NOTE: Counting default material for visibility rendering.
						{0, {1}},
					},
					.rigidBodyMappings = {
						// Player
						{
							0,
							createRigidBody({
								.position = {0.0f, 38.5f, 0.0f},
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