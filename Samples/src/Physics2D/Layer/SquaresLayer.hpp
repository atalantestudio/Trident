#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class SquaresLayer : public BaseLayer {
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

				const Entity entityA = scene.createPhysicalObjectEntity(0);

				ref<RigidBody2D> bodyA = createRef<RigidBody2D>();
				bodyA->position = {-3.0f, -3.0f};
				bodyA->linearVelocity = {0.02f, 0.02f};

				ref<ColorMaterial> materialA = createRef<ColorMaterial>();
				materialA->fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
				materialA->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityA, std::static_pointer_cast<Body2D>(bodyA));
				scene.setGeometry(entityA, std::static_pointer_cast<Geometry2D>(squareGeometry));
				scene.setMaterial(entityA, std::static_pointer_cast<Material>(materialA));
				scene.updateMass(entityA);
				scene.updateTransform(entityA);

				const Entity entityB = scene.createPhysicalObjectEntity(0);

				ref<RigidBody2D> bodyB = createRef<RigidBody2D>();
				bodyB->position = {3.0f, -3.0f};
				bodyB->linearVelocity = {-0.02f, 0.02f};

				ref<ColorMaterial> materialB = createRef<ColorMaterial>();
				materialB->fillColor = {0.902f, 0.867f, 1.0f, 1.0f};
				materialB->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityB, std::static_pointer_cast<Body2D>(bodyB));
				scene.setGeometry(entityB, std::static_pointer_cast<Geometry2D>(squareGeometry));
				scene.setMaterial(entityB, std::static_pointer_cast<Material>(materialB));
				scene.updateMass(entityB);
				scene.updateTransform(entityB);

				const Entity entityC = scene.createPhysicalObjectEntity(0);

				ref<RigidBody2D> bodyC = createRef<RigidBody2D>();
				bodyC->position = {-3.0f, 3.0f};
				bodyC->linearVelocity = {0.02f, -0.02f};

				ref<ColorMaterial> materialC = createRef<ColorMaterial>();
				materialC->fillColor = {0.902f, 0.867f, 1.0f, 1.0f};
				materialC->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityC, std::static_pointer_cast<Body2D>(bodyC));
				scene.setGeometry(entityC, std::static_pointer_cast<Geometry2D>(squareGeometry));
				scene.setMaterial(entityC, std::static_pointer_cast<Material>(materialC));
				scene.updateMass(entityC);
				scene.updateTransform(entityC);

				const Entity entityD = scene.createPhysicalObjectEntity(0);

				ref<RigidBody2D> bodyD = createRef<RigidBody2D>();
				bodyD->position = {3.0f, 3.0f};
				bodyD->linearVelocity = {-0.02f, -0.02f};

				ref<ColorMaterial> materialD = createRef<ColorMaterial>();
				materialD->fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
				materialD->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityD, std::static_pointer_cast<Body2D>(bodyD));
				scene.setGeometry(entityD, std::static_pointer_cast<Geometry2D>(squareGeometry));
				scene.setMaterial(entityD, std::static_pointer_cast<Material>(materialD));
				scene.updateMass(entityD);
				scene.updateTransform(entityD);

				renderer.build();
				renderer.registerScene(scene);
			}

			void PhysicsUpdate() override {
				static const vec2<float32> g = {0.0_m_s2, -0.000980665_m_s2};

				for (Entity entity = 0; entity < scene.getPhysicalObjectEntityCount(); entity += 1) {
					ref<Body2D>& body = scene.getBody(entity);

					body->applyLinearForce(g);
				}
			}
	};
}