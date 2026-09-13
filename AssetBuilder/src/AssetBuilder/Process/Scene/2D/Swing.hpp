// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"

namespace Trident {
	class SwingScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_swing";
			}

			BuildScene2D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticGeometry = {},
					.dynamicGeometry = {
						.entityCount = 6,
						.geometryAssetIdentifiers = {
							assetLibrary->getAsset<Geometry2DAsset>("2d_unit_square")->identifier,
						},
						.geometryMappings = {
							{0, {0}},
							{1, {0}},
							{2, {0}},
							{3, {0}},
							{4, {0}},
							{5, {0}},
						},
						.rigidBodyMappings = {
							// Floor
							{
								0,
								createRigidBody({
									.position = {0.0_m, -10.0_m},
									.rotation = 0.0_rad,
									.scale = {100.0f, 20.0f},
									.mass = max<float32>(),
									.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
									.friction = 0.2f,
									.restitution = 0.0f,
								}),
							},

							// Swing base
							{
								1,
								createRigidBody({
									.position = {0.0_m, 0.5_m},
									.rotation = 0.0_rad,
									.scale = {0.1f, 1.0f},
									.mass = max<float32>(),
									.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
									.friction = 0.0f,
									.restitution = 0.0f,
								}),
							},

							// Swing
							{
								2,
								createRigidBody({
									.position = {0.0_m, 1.0_m},
									.rotation = 0.0_rad,
									.scale = {12.0f, 0.25f},
									.mass = 100.0_kg,
									.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
									.friction = 0.2f,
									.restitution = 0.0f,
								}),
							},

							// Left weight 1
							{
								3,
								createRigidBody({
									.position = {-5.0_m, 2.0_m},
									.rotation = 0.0_rad,
									.scale = {0.5f, 0.5f},
									.mass = 25.0_kg,
									.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
									.friction = 0.2f,
									.restitution = 0.0f,
								}),
							},

							// Left weight 2
							{
								4,
								createRigidBody({
									.position = {-5.5_m, 2.0_m},
									.rotation = 0.0_rad,
									.scale = {0.5f, 0.5f},
									.mass = 25.0_kg,
									.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
									.friction = 0.2f,
									.restitution = 0.0f,
								}),
							},

							// Right weight
							{
								5,
								createRigidBody({
									.position = {5.5_m, 15.0_m},
									.rotation = 0.0_rad,
									.scale = {1.0f, 1.0f},
									.mass = 100.0_kg,
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
			using Scene2DProcess::Scene2DProcess;
	};
}