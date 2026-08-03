// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"
#include "../../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class SlidingScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_sliding";
			}

			Scene2DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr uint32 entityCount = 11;
				static constexpr float32 frictions[] = {0.75f, 0.5f, 0.35f, 0.1f, 0.0f};

				sequence<EntityComponentMapping<GeometryComponent>> geometryMappings(entityCount);

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
						.position = {-2.0_m, 11.0_m},
						.rotation = 0.25_rad,
						.scale = {13.0f, 0.25f},
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
						.position = {5.25_m, 9.5_m},
						.rotation = 0.0_rad,
						.scale = {0.25f, 1.0f},
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
						.position = {2.0_m, 7.0_m},
						.rotation = -0.25_rad,
						.scale = {13.0f, 0.25f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				};

				// Obstacle 4
				rigidBodyMappings[4] = {
					4,
					createRigidBody({
						.position = {-5.25_m, 5.5_m},
						.rotation = 0.0_rad,
						.scale = {0.25f, 1.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				};

				// Obstacle 5
				rigidBodyMappings[5] = {
					5,
					createRigidBody({
						.position = {-2.0_m, 3.0_m},
						.rotation = 0.25_rad,
						.scale = {13.0f, 0.25f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				};

				Descriptor<RigidBody2DComponent> boxDescriptor = {
					.rotation = 0.0_rad,
					.scale = {0.5f, 0.5f},
					.mass = 25.0_kg,
					.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					.friction = 0.2f,
					.restitution = 0.0f,
				};

				uint32 boxIndex = 0;

				while (boxIndex < 5) {
					boxDescriptor.position = {-7.5_m + boxIndex * 2.0f, 14.0_m};
					boxDescriptor.friction = frictions[boxIndex];

					rigidBodyMappings[boxIndex + 6] = {boxIndex + 6, createRigidBody(boxDescriptor)};

					boxIndex += 1;
				}

				return {
					.staticScene = {},
					.entityCount = entityCount,
					.geometryAssetIdentifiers = {35},
					.geometryMappings = geometryMappings,
					.rigidBodyMappings = rigidBodyMappings,
				};
			}

		public:
			explicit SlidingScene2D(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}