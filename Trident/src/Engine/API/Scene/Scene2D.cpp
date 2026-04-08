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
		Scene({
			.staticVertexCount = sceneAsset->getStaticVertexCount(),
			.staticIndexCount = sceneAsset->getStaticIndexCount(),
			.dynamicVertexCount = sceneAsset->getDynamicVertexPositionCount() + sceneAsset->getDynamicVertexTextureCoordinateCount(),
			.dynamicIndexCount = sceneAsset->getDynamicVertexPositionIndexCount() + sceneAsset->getDynamicVertexTextureCoordinateIndexCount()
		})
	{
		loadGeometries(sceneAsset);
		loadEntityComponentMappings(sceneAsset);
	}

	void Scene2D::loadGeometries(ref<Scene2DAsset> sceneAsset) {
		const sequence<uint8>& staticBuffer = sceneAsset->getStaticBuffer();
		const sequence<ref<GeometryAsset>>& geometryAssets = sceneAsset->getGeometryAssets();
		const uint32 vertexComponentCount = static_cast<uint32>(staticVertexCount + dynamicVertexCount * 2);
		const uint64 vertexByteCount = staticVertexCount + dynamicVertexCount * sizeof(svec2<float32>);
		const uint64 indexByteCount = staticIndexCount + dynamicIndexCount * sizeof(uint32);

		dataBuffer = sequence<uint8>(vertexByteCount + indexByteCount);
		geometries = sequence<Geometry2D>(geometryAssets.count());
		hulls = sequence<Hull2D>(sceneAsset->getHullCount());

		std::copy(staticBuffer.begin(), staticBuffer.end(), &dataBuffer[0]);

		uint8* dynamicDataBuffer = &dataBuffer[staticBuffer.count()];
		uint64 offset = 0;

		range<uint32, uint32> vertexPositionComponentRange = {static_cast<uint32>(offset), 0};

		for (const ref<GeometryAsset> geometryAsset : geometryAssets) {
			const sequence<float32>& vertexPositionComponents = geometryAsset->getVertexPositionComponents();

			std::copy(vertexPositionComponents.begin(), vertexPositionComponents.end(), &pointer_cast<float32*>(dynamicDataBuffer)[offset]);

			offset += vertexPositionComponents.count();
		}

		range<uint32, uint32> vertexTextureCoordinateComponentRange = {static_cast<uint32>(offset), 0};

		for (const ref<GeometryAsset> geometryAsset : geometryAssets) {
			const sequence<float32>& vertexTextureCoordinateComponents = geometryAsset->getVertexTextureCoordinateComponents();

			std::copy(vertexTextureCoordinateComponents.begin(), vertexTextureCoordinateComponents.end(), &pointer_cast<float32*>(dynamicDataBuffer)[offset]);

			offset += vertexTextureCoordinateComponents.count();
		}

		range<uint32, uint32> vertexPositionIndexRange = {static_cast<uint32>(offset), 0};

		for (const ref<GeometryAsset> geometryAsset : geometryAssets) {
			const sequence<uint32>& vertexPositionIndices = geometryAsset->getVertexPositionIndices();

			std::copy(vertexPositionIndices.begin(), vertexPositionIndices.end(), &pointer_cast<uint32*>(dynamicDataBuffer)[offset]);

			offset += vertexPositionIndices.count();
		}

		uint32 geometryIndex = 0;
		uint32 hullIndex = 0;

		while (geometryIndex < geometries.count()) {
			const ref<GeometryAsset> geometryAsset = geometryAssets[geometryIndex];

			vertexPositionComponentRange.count = static_cast<uint32>(geometryAsset->getVertexPositionComponents().count());
			vertexTextureCoordinateComponentRange.count = static_cast<uint32>(geometryAsset->getVertexTextureCoordinateComponents().count());
			vertexPositionIndexRange.count = static_cast<uint32>(geometryAsset->getVertexPositionIndices().count());

			geometries[geometryIndex] = Geometry2D({
				.vertexPositionComponentRange = {vertexPositionComponentRange.offset, vertexPositionComponentRange.count},
				.vertexTextureCoordinateComponentRange = {vertexTextureCoordinateComponentRange.offset, vertexTextureCoordinateComponentRange.count},
				.indexRange = {vertexPositionIndexRange.offset - vertexComponentCount, vertexPositionIndexRange.count},
			});

			if (geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL) {
				const sequence<uint32>& hullLineIndices = geometryAsset->getHullLineIndices();

				std::copy(hullLineIndices.begin(), hullLineIndices.end(), &pointer_cast<uint32*>(dynamicDataBuffer)[offset]);

				offset += hullLineIndices.count();

				const view<svec2<float32>> vertices(pointer_cast<const svec2<float32>*>(&dynamicDataBuffer[vertexPositionComponentRange.offset * sizeof(float32)]), vertexPositionComponentRange.count / 2);

				hulls[hullIndex] = Hull2D(geometryAsset, vertices, vertexPositionIndexRange.offset + vertexPositionIndexRange.count - vertexComponentCount);

				geometries[geometryIndex].hullIndex = hullIndex;

				hullIndex += 1;
			}

			vertexPositionComponentRange.offset += vertexPositionComponentRange.count;
			vertexTextureCoordinateComponentRange.offset += vertexTextureCoordinateComponentRange.count;
			vertexPositionIndexRange.offset += vertexPositionIndexRange.count;

			geometryIndex += 1;
		}
	}

	void Scene2D::loadEntityComponentMappings(ref<Scene2DAsset> sceneAsset) {
		// TODO(ecs/scene-components): Transform2DComponent/Transform3DComponent for static meshes?
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

			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(rigidBodyMapping.entity);
			const Geometry2D& geometry = geometries[geometryComponent.geometryIndex];
			const Hull2D& hull = hulls[geometry.hullIndex];

			ecs.getComponent<RigidBody2DComponent>(rigidBodyMapping.entity).updateWorld(hull.getCentroid());

			// TODO(debug-only)
			if (rigidBodyMapping.component.inverseMass == 0.0_kg) {
				ecs.setTag<StaticEntityTag>(rigidBodyMapping.entity);
			}
			else {
				ecs.setTag<DynamicEntityTag>(rigidBodyMapping.entity);
			}
		}
	}

	// TODO: Restore joint constraints.
	/* void Scene2D::createJoint(const Joint2DDescriptor& descriptor) {
		const Entity entity = ecs.createEntity();
		const RigidBody2DComponent& rigidBodyA = ecs.getComponent<RigidBody2DComponent>(descriptor.A);
		const RigidBody2DComponent& rigidBodyB = ecs.getComponent<RigidBody2DComponent>(descriptor.B);
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