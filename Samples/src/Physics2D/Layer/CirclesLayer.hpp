#pragma once

#include "BaseLayer.hpp"

namespace ProjectA {
	class CirclesLayer : public BaseLayer {
		public:
			void build() override {
				CircleGeometry2DDescriptor circleGeometryDescriptor;
				circleGeometryDescriptor.radius = 3.0f;
				circleGeometryDescriptor.resolution = 16;

				const ref<CircleGeometry2D> circleGeometry = createRef<CircleGeometry2D>(circleGeometryDescriptor);

				const Entity entityA = scene.createDynamicEntity(0);

				ref<RigidBody2D> bodyA = createRef<RigidBody2D>();

				ref<ColorMaterial> materialA = createRef<ColorMaterial>();
				materialA->fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
				materialA->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityA, std::static_pointer_cast<Body2D>(bodyA));
				scene.setGeometry(entityA, std::static_pointer_cast<Geometry2D>(circleGeometry));
				scene.setMaterial(entityA, std::static_pointer_cast<Material>(materialA));
				scene.updateMass(entityA);
				scene.updateTransform(entityA);

				renderer.build();
				renderer.registerScene(scene);
			}
	};
}