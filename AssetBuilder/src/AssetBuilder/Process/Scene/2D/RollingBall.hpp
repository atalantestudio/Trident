// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"
#include "../../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class RollingBallScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_rolling_ball";
			}

			Scene2DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 4,
					.geometryAssetIdentifiers = {
						35,
						34,
					},
					.geometryMappings = {
						{0, {0}},
						{1, {0}},
						{2, {0}},
						{3, {1}},
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

						// Slide 1
						{
							1,
							createRigidBody({
								.position = {-4.0_m, 3.2_m},
								.rotation = radians(20.0_deg),
								.scale = {20.0f, 0.5f},
								.mass = max<float32>(),
								.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},

						// Slide 2
						{
							2,
							createRigidBody({
								.position = {8.0_m, 0.5_m},
								.rotation = radians(-20.0_deg),
								.scale = {5.0f, 0.5f},
								.mass = max<float32>(),
								.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},

						// Ball
						{
							3,
							createRigidBody({
								.position = {-13.0_m, 10.0_m},
								.rotation = 0.0_rad,
								.scale = {1.0f, 1.0f},
								.mass = 25.0_kg,
								.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},
					},
				};
			}

		public:
			explicit RollingBallScene2D(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}