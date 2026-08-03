// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"
#include "../../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class PolygonsScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_polygons";
			}

			Scene2DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 3,
					.geometryAssetIdentifiers = {
						35,

						// TODO(debug-only): Polygon
						37,

						// TODO(debug-only): Triangle
						36,
					},
					.geometryMappings = {
						{0, {0}},
						{1, {1}},
						{2, {2}},
					},
					.rigidBodyMappings = {
						// Floor
						{
							0,
							createRigidBody({
								.position = {0.0_m, -15.0_m},
								.rotation = 0.0_rad,
								.scale = {100.0f, 20.0f},
								.mass = max<float32>(),
								.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},

						// Polygon
						{
							1,
							createRigidBody({
								.position = {0.45_m, -0.625_m},
								.rotation = 0.0_rad,
								.scale = {1.0f, 1.0f},
								.mass = max<float32>(),
								.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},

						// Triangle
						{
							2,
							createRigidBody({
								.position = {0.0_m, 12.0_m},
								// .rotation = 0.0_rad,
								.rotation = radians(-165.0_deg),
								.scale = {1.0f, 1.0f},
								.mass = 1.0_kg,
								.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},
					},
				};
			}

		public:
			explicit PolygonsScene2D(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}