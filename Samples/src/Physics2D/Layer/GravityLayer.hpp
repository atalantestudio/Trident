#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class GravityLayer : public BaseLayer {
		public:
			void build() override {
				ConvexHullGeometry2DDescriptor squareGeometryDescriptor;
				squareGeometryDescriptor.vertices = {
					{-1.0f, -1.0f},
					{-1.0f,  1.0f},
					{ 1.0f,  1.0f},
					{ 1.0f, -1.0f},
				};
				squareGeometryDescriptor.indices = {
					0, 1, 2,
					0, 2, 3,
				};

				const ref<ConvexHullGeometry2D> squareGeometry = createRef<ConvexHullGeometry2D>(squareGeometryDescriptor);

				const Entity floor = scene.createStaticEntity(0);

				ref<RigidBody2D> floorBody = createRef<RigidBody2D>();
				floorBody->position = {0.0f, -3.0f};
				floorBody->scale = {8.0f, 1.0f};
				floorBody->inverseInertia = 0.0f;

				ref<ColorMaterial> floorMaterial = createRef<ColorMaterial>();
				floorMaterial->fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
				floorMaterial->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};
				floorMaterial->density = 0.0f;

				scene.setBody(floor, std::static_pointer_cast<Body2D>(floorBody));
				scene.setGeometry(floor, std::static_pointer_cast<Geometry2D>(squareGeometry));
				scene.setMaterial(floor, std::static_pointer_cast<Material>(floorMaterial));
				scene.updateMass(floor);
				scene.updateTransform(floor);

				const Entity box = scene.createDynamicEntity(0);

				ref<RigidBody2D> boxBody = createRef<RigidBody2D>();
				boxBody->position = {2.0f, 3.0f};
				boxBody->linearVelocity = {0.0f, -0.02f};
				// boxBody->rotation = lyah::radians(40.0f);
				boxBody->inverseInertia = 0.0f;
				boxBody->scale = {3.0f, 1.0f};

				ref<ColorMaterial> boxMaterial = createRef<ColorMaterial>();
				boxMaterial->fillColor = {0.902f, 0.867f, 1.0f, 1.0f};
				boxMaterial->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};
				// boxMaterial->density = 0.05f;
				boxMaterial->coefficientOfRestitution = 1.0f;

				scene.setBody(box, std::static_pointer_cast<Body2D>(boxBody));
				scene.setGeometry(box, std::static_pointer_cast<Geometry2D>(squareGeometry));
				scene.setMaterial(box, std::static_pointer_cast<Material>(boxMaterial));
				scene.updateMass(box);
				scene.updateTransform(box);

				renderer.build();
				renderer.registerScene(scene);
			}

			void PhysicsUpdate() override {
				static const vec2<float32> g = {0.0_m_s2, -0.000980665_m_s2};

				for (Entity entity = 0; entity < scene.getPhysicalObjectEntityCount(); entity += 1) {
					ref<Body2D>& body = scene.getBody(entity);

					// body->applyLinearForce(g);
				}
			}
	};
}