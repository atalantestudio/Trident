#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class Demo1Layer : public BaseLayer {
		private:
			static constexpr float32 BOX2D_LITE_DEMO_SCALE_MULTIPLIER = 0.4f;
		public:
			void build() override {
				ConvexHullGeometry2DDescriptor polygonGeometryDescriptor;
				polygonGeometryDescriptor.vertices = {
					{-1.0f, -1.0f},
					{-1.0f,  1.0f},
					{ 1.0f,  1.0f},
					{ 1.0f, -1.0f},
				};
				polygonGeometryDescriptor.indices = {
					0, 1, 2,
					0, 2, 3,
				};

				const ref<ConvexHullGeometry2D> polygonGeometry = createRef<ConvexHullGeometry2D>(polygonGeometryDescriptor);

				const Entity entityA = scene.createDynamicEntity(0);

				ref<RigidBody2D> bodyA = createRef<RigidBody2D>();
				bodyA->position = vec2<float32>(0.0f, -0.5f * 20.0f);
				bodyA->scale = vec2<float32>(100.0f, 20.0f) * BOX2D_LITE_DEMO_SCALE_MULTIPLIER;
				bodyA->inverseMass = 0_kg;
				bodyA->inverseInertia = 0.0f;

				ref<ColorMaterial> materialA = createRef<ColorMaterial>();
				materialA->fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
				materialA->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityA, std::static_pointer_cast<Body2D>(bodyA));
				scene.setGeometry(entityA, std::static_pointer_cast<Geometry2D>(polygonGeometry));
				scene.setMaterial(entityA, std::static_pointer_cast<Material>(materialA));
				scene.updateTransform(entityA);

				const Entity entityB = scene.createDynamicEntity(0);

				ref<RigidBody2D> bodyB = createRef<RigidBody2D>();
				bodyB->position = vec2<float32>(0.0f, 4.0f);
				bodyB->scale = vec2<float32>(1.0f, 1.0f) * BOX2D_LITE_DEMO_SCALE_MULTIPLIER;
				bodyB->inverseMass = rcp(200_kg * BOX2D_LITE_DEMO_SCALE_MULTIPLIER);
				bodyB->inverseInertia = 200_kg * BOX2D_LITE_DEMO_SCALE_MULTIPLIER / 12.0f;

				ref<ColorMaterial> materialB = createRef<ColorMaterial>();
				materialB->fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
				materialB->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityB, std::static_pointer_cast<Body2D>(bodyB));
				scene.setGeometry(entityB, std::static_pointer_cast<Geometry2D>(polygonGeometry));
				scene.setMaterial(entityB, std::static_pointer_cast<Material>(materialB));
				scene.updateTransform(entityB);

				renderer.build();
				renderer.registerScene(scene);
			}

			void physicsUpdate() override {
				static const vec2<float32> g = {0.0_m_s2, -0.000980665_m_s2};

				for (Entity entity = 0; entity < scene.getPhysicalObjectEntityCount(); entity += 1) {
					ref<Body2D>& body = scene.getBody(entity);

					body->applyForce(g);
				}
			}
	};
}