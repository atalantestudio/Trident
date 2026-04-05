// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Tag.hpp"

namespace Trident {
	Scene2D::Scene2D(ref<Scene2DAsset> sceneAsset) :
		staticVertexCount(sceneAsset->getStaticVertexCount()),
		staticIndexCount(sceneAsset->getStaticIndexCount())
	{
		loadGeometries(sceneAsset);

		// TODO: Load hulls.
		// loadHulls(sceneAsset);

		loadEntityComponentMappings(sceneAsset);
	}

	void Scene2D::loadGeometries(ref<Scene2DAsset> sceneAsset) {
		const sequence<uint8>& staticBuffer = sceneAsset->getStaticBuffer();
		const sequence<ref<GeometryAsset>>& geometryAssets = sceneAsset->getGeometryAssets();

		geometries = sequence<Geometry2D>(geometryAssets.count());

		uint32 geometryIndex = 0;

		while (geometryIndex < geometries.count()) {
			const ref<GeometryAsset> geometryAsset = geometryAssets[geometryIndex];

			geometries[geometryIndex] = Geometry2D({
				.vertexPositionComponents = geometryAsset->getVertexPositionComponents(),
				.vertexTextureCoordinateComponents = geometryAsset->getVertexTextureCoordinateComponents(),
				.indices = geometryAsset->getVertexPositionIndices(),

				// TODO: Provide correct offset.
				.indexOffset = 0,
			});

			dynamicVertexCount += geometries[geometryIndex].getVertexCount();
			dynamicIndexCount += static_cast<uint32>(geometries[geometryIndex].getIndices().count());

			geometryIndex += 1;
		}

		// TODO: Restore texture coordinates.
		dataBuffer = sequence<uint8>((staticVertexCount + dynamicVertexCount) * /* 4 */ 2 * sizeof(float32) + (staticIndexCount + dynamicIndexCount) * sizeof(uint32));

		uint64 dataOffset = 0;

		std::copy(staticBuffer.begin(), staticBuffer.end(), &dataBuffer[dataOffset]);

		dataOffset += staticBuffer.count();

		geometryIndex = 0;

		while (geometryIndex < geometries.count()) {
			const Geometry2D& geometry = geometries[geometryIndex];
			const sequence<float32>& vertexPositionComponents = geometry.getVertexPositionComponents();

			std::copy(vertexPositionComponents.begin(), vertexPositionComponents.end(), pointer_cast<float32*>(&dataBuffer[dataOffset]));

			dataOffset += vertexPositionComponents.count() * sizeof(float32);
			geometryIndex += 1;
		}

		// TODO: Restore texture coordinates.
		/* geometryIndex = 0;

		while (geometryIndex < geometries.count()) {
			const Geometry2D& geometry = geometries[geometryIndex];
			const sequence<float32>& vertexTextureCoordinateComponents = geometry.getVertexTextureCoordinateComponents();

			std::copy(vertexTextureCoordinateComponents.begin(), vertexTextureCoordinateComponents.end(), pointer_cast<float32*>(&dataBuffer[dataOffset]));

			dataOffset += vertexTextureCoordinateComponents.count() * sizeof(float32);
			geometryIndex += 1;
		} */

		geometryIndex = 0;

		while (geometryIndex < geometries.count()) {
			const Geometry2D& geometry = geometries[geometryIndex];
			const sequence<uint32>& indices = geometry.getIndices();

			std::copy(indices.begin(), indices.end(), pointer_cast<uint32*>(&dataBuffer[dataOffset]));

			dataOffset += indices.count() * sizeof(uint32);
			geometryIndex += 1;
		}
	}

	void Scene2D::loadHulls(ref<Scene2DAsset> sceneAsset) {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	void Scene2D::loadEntityComponentMappings(ref<Scene2DAsset> sceneAsset) {
		// TODO(ecs/scene-components):
		// - MaterialReferenceComponent
		// - RigidBody3DComponent
		// - Transform2DComponent/Transform3DComponent for static meshes?

		ecs.registerComponentPool<GeometryComponent>();
		ecs.registerComponentPool<RigidBody2DComponent>();
		ecs.registerComponentPool<Joint2D>();
		ecs.registerComponentPool<ref<Constraint2D>>();
		ecs.registerTagPool<DynamicEntityTag>();
		ecs.registerTagPool<StaticEntityTag>();

		Entity entity = 0;

		while (entity < sceneAsset->getEntityCount()) {
			ecs.createEntity();

			entity += 1;
		}

		for (const EntityComponentMapping<GeometryComponent> geometryMapping : sceneAsset->getGeometryMappings()) {
			ecs.setComponent<GeometryComponent>(geometryMapping.entity, geometryMapping.component);
		}

		for (const EntityComponentMapping<RigidBody2DComponent>& rigidBodyMapping : sceneAsset->getRigidBodyMappings()) {
			ecs.setComponent<RigidBody2DComponent>(rigidBodyMapping.entity, rigidBodyMapping.component);

			// TODO(debug-only)
			ecs.getComponent<RigidBody2DComponent>(rigidBodyMapping.entity).updateTransform({0.0f, 0.0f});
			ecs.setTag<DynamicEntityTag>(entity);
		}
	}

	// TODO: Restore joint constraints.
	/* void Scene2D::createJoint(const Joint2DDescriptor& descriptor) {
		const Entity entity = ecs.createEntity();
		const RigidBody2D& rigidBodyA = ecs.getComponent<RigidBody2D>(descriptor.A);
		const RigidBody2D& rigidBodyB = ecs.getComponent<RigidBody2D>(descriptor.B);
		const mat2x2<float32> RA = transpose(mat2x2<float32>::rotation(rigidBodyA.rotation));
		const mat2x2<float32> RB = transpose(mat2x2<float32>::rotation(rigidBodyB.rotation));

		Joint2D joint{};
		joint.A = descriptor.A;
		joint.B = descriptor.B;
		joint.localAnchorA = (descriptor.anchor - rigidBodyA.position) * RA;
		joint.localAnchorB = (descriptor.anchor - rigidBodyB.position) * RB;
		joint.biasFactor = descriptor.biasFactor;
		joint.softness = descriptor.softness;

		ecs.setComponent<Joint2D>(entity, joint);
	} */
}