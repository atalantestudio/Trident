#pragma once

#include "Engine/API/Geometry/PolytopeGeometry2D.hpp"
#include "Engine/API/Material/ColorMaterial.hpp"

#include "BaseLayer.hpp"

namespace ProjectA {
	class PolygonsLayer : public BaseLayer {
		public:
			void build() override {
				Geometry2DDescriptor polygonGeometryDescriptor;
				polygonGeometryDescriptor.vertices = {
					{-3.0f,   0.0f},
					{ 4.0f,   0.0f},
					{ 3.0f,  -1.0f},
					{-2.25f, -1.5f},
				};
				polygonGeometryDescriptor.indices = {
					0, 1, 2,
					0, 2, 3,
				};

				Geometry2DDescriptor triangleGeometryDescriptor;
				triangleGeometryDescriptor.vertices = {
					{0.0f, 0.0f},
					{0.0f, 1.0f},
					{6.0f, 0.0f},
				};
				triangleGeometryDescriptor.indices = {
					0, 1, 2,
				};

				const ref<PolytopeGeometry2D> polygonGeometry = createRef<PolytopeGeometry2D>(polygonGeometryDescriptor);
				const ref<PolytopeGeometry2D> triangleGeometry = createRef<PolytopeGeometry2D>(triangleGeometryDescriptor);

				const Entity entityA = scene.createPhysicalObjectEntity();

				ref<RigidBody2D> bodyA = createRef<RigidBody2D>();
				bodyA->position = {0.45f, -0.625f};

				ref<ColorMaterial> materialA = createRef<ColorMaterial>();
				materialA->density = 1.0f;
				materialA->coefficientOfRestitution = 1.0f;
				materialA->fillColor = {0.957f, 0.997f, 0.859f, 1.0f};
				materialA->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityA, std::static_pointer_cast<Body>(bodyA));
				scene.setGeometry(entityA, std::static_pointer_cast<Geometry2D>(polygonGeometry));
				scene.setMaterial(entityA, std::static_pointer_cast<Material>(materialA));
				scene.updateMass(entityA);
				scene.updateTransform(entityA);

				const Entity entityB = scene.createPhysicalObjectEntity();

				ref<RigidBody2D> bodyB = createRef<RigidBody2D>();
				bodyB->position = {0.0f, 1.957f};
				bodyB->rotation = lyah::radians(-30.0f);
				bodyB->angularVelocity = lyah::radians(0.4617f);

				ref<ColorMaterial> materialB = createRef<ColorMaterial>();
				materialB->density = 1.0f;
				materialB->coefficientOfRestitution = 1.0f;
				materialB->fillColor = {0.902f, 0.867f, 1.0f, 1.0f};
				materialB->strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};

				scene.setBody(entityB, std::static_pointer_cast<Body>(bodyB));
				scene.setGeometry(entityB, std::static_pointer_cast<Geometry2D>(triangleGeometry));
				scene.setMaterial(entityB, std::static_pointer_cast<Material>(materialB));
				scene.updateMass(entityB);
				scene.updateTransform(entityB);

				renderer.build();
				renderer.registerScene(scene);
			}
	};
}