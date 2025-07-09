#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class SquaresLayer : public BaseLayer {
		public:
			void build() override {
				Geometry2DDescriptor squareGeometryDescriptor;
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

				const ref<PolytopeGeometry2D> squareGeometry = createRef<PolytopeGeometry2D>(squareGeometryDescriptor);

				const Entity entityA = scene.createPhysicalObjectEntity(0);

				ref<RigidBody2D> bodyA = createRef<RigidBody2D>();
				bodyA->position = {-3.0f, -3.0f};
				bodyA->linearVelocity = {0.02f, 0.02f};

				ref<ColorMaterial> materialA = createRef<ColorMaterial>();
				materialA->density = 1.0f;
				materialA->coefficientOfRestitution = 1.0f;
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
				materialB->density = 1.0f;
				materialB->coefficientOfRestitution = 1.0f;
				materialB->fillColor = {0.902f, 0.867f, 1.0f, 1.0f};
				materialB->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityB, std::static_pointer_cast<Body2D>(bodyB));
				scene.setGeometry(entityB, std::static_pointer_cast<Geometry2D>(squareGeometry));
				scene.setMaterial(entityB, std::static_pointer_cast<Material>(materialB));
				scene.updateMass(entityB);
				scene.updateTransform(entityB);

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