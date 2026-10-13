// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"

namespace Trident {
	class DominosScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_dominos";
			}

			BuildScene2D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr uint32 entityCount = 15;

				sequence<EntityComponentMapping<Geometry2DComponent>> geometryMappings(entityCount);

				Entity entity = 0;

				while (entity < geometryMappings.count()) {
					geometryMappings[entity] = {entity, {0}};

					entity += 1;
				}

				sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings(entityCount);

				// Floor
				rigidBodyMappings[0] = {
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
				};

				// Obstacle 1
				rigidBodyMappings[1] = {
					1,
					createRigidBody({
						.position = {-1.5_m, 10.0_m},
						.rotation = 0.0_rad,
						.scale = {12.0f, 0.5f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				};

				// Obstacle 2
				rigidBodyMappings[2] = {
					2,
					createRigidBody({
						.position = {1.0_m, 6.0_m},
						.rotation = -0.3_rad,
						.scale = {14.0f, 0.5f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				};

				// Obstacle 3
				rigidBodyMappings[3] = {
					3,
					createRigidBody({
						.position = {-7.0_m, 4.0_m},
						.rotation = 0.0_rad,
						.scale = {0.5f, 3.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				};

				// Pusher
				rigidBodyMappings[4] = {
					4,
					createRigidBody({
						.position = {-7.5_m, 12.0_m},
						.rotation = 0.0_rad,
						.scale = {0.5f, 2.0f},
						.linearVelocity = {4.0_m_s, 0.0_m_s},
						.mass = 10.0_kg,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				};

				Descriptor<RigidBody2DComponent> dominoDescriptor = {
					.rotation = 0.0_rad,
					.scale = {0.2_m, 2.0_m},
					.mass = 10.0_kg,
					.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					.friction = 0.1f,
					.restitution = 0.0f,
				};

				uint32 dominoIndex = 0;

				while (dominoIndex < 10) {
					dominoDescriptor.position = {-6.0_m + dominoIndex, 11.125_m};

					rigidBodyMappings[dominoIndex + 5] = {dominoIndex + 5, createRigidBody(dominoDescriptor)};

					dominoIndex += 1;
					entity += 1;
				}

				return {
					.staticGeometry = {},
					.dynamicGeometry = {
						.entityCount = entityCount,
						.geometryAssetIdentifiers = {
							assetLibrary->getAsset<Geometry2DAsset>("2d_unit_square")->identifier,
						},
						.geometryMappings = geometryMappings,
						.rigidBodyMappings = rigidBodyMappings,
					},
				};
			}

		public:
			using Scene2DProcess::Scene2DProcess;
	};
}