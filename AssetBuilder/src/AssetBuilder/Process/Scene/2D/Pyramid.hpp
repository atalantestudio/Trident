// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"
#include "../../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class PyramidScene2D final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_pyramid";
			}

			Scene2DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr uint32 entityCount = 79;

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

				vec2<float32> x = {-6.0_m, 0.75_m};
				vec2<float32> y;
				uint32 i = 0;
				uint32 j;

				entity = 1;

				while (i < 12) {
					y = x;
					j = i;

					while (j < 12) {
						boxDescriptor.position = svec2<float32>(y);

						rigidBodyMappings[entity] = {entity, createRigidBody(boxDescriptor)};

						entity += 1;

						y += {1.125_m, 0.0_m};
						j += 1;
					}

					x += {0.5625_m, 2.0_m};
					i += 1;
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
			explicit PyramidScene2D(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}