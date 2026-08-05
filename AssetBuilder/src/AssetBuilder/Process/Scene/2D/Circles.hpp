// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"
#include "../../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class CirclesScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_circles";
			}

			Scene2DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 3,
					.geometryAssetIdentifiers = {
						assetLibrary->getAsset<Geometry2DAsset>("UnitCircle")->identifier,
					},
					.geometryMappings = {
						{0, {0}},
						{1, {0}},
						{2, {0}},
					},
					.rigidBodyMappings = {
						// Static circle 1
						{
							0,
							createRigidBody({
								.position = {-10.0_m, -10.0_m},
								.rotation = 0.0_rad,
								.scale = {20.0f, 20.0f},
								.mass = max<float32>(),
								.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},

						// Static circle 2
						{
							1,
							createRigidBody({
								.position = {10.0_m, -10.0_m},
								.rotation = 0.0_rad,
								.scale = {20.0f, 20.0f},
								.mass = max<float32>(),
								.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},

						// Dynamic circle
						{
							2,
							createRigidBody({
								.position = {9.5_m, 1.0_m},
								.rotation = 0.0_rad,
								.scale = {1.0f, 1.0f},
								.mass = 200.0_kg,
								.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},
					},
				};
			}

		public:
			explicit CirclesScene2D(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}