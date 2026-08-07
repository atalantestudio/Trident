// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"

namespace Trident {
	class PolygonsScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_polygons";
			}

			BuildScene2D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticGeometry = {},
					.dynamicGeometry = {
						.entityCount = 3,
						.geometryAssetIdentifiers = {
							assetLibrary->getAsset<Geometry2DAsset>("2d_unit_square")->identifier,
							assetLibrary->getAsset<Geometry2DAsset>("2d_polygon")->identifier,
							assetLibrary->getAsset<Geometry2DAsset>("2d_triangle")->identifier,
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
					},
				};
			}

		public:
			explicit PolygonsScene2D(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}