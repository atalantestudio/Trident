// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"
#include "../../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class StackingScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_stacking";
			}

			Scene2DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr uint32 entityCount = 11;

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

				Descriptor<RigidBody2DComponent> boxDescriptor = {
					.rotation = 0.0_rad,
					.scale = {1.0f, 1.0f},
					.mass = 10.0_kg,
					.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					.friction = 0.2f,
					.restitution = 0.0f,
				};

				uint32 boxIndex = 0;

				while (boxIndex < 10) {
					boxDescriptor.position = {0.0_m, 0.51_m + boxIndex * 1.05f};

					rigidBodyMappings[boxIndex + 1] = {boxIndex + 1, createRigidBody(boxDescriptor)};

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
			explicit StackingScene2D(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}