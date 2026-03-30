// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"
#include "Engine/API/Physics/Cached/CachedRigidBody2D.hpp"
#include "Engine/API/Process/Process.hpp"

namespace Trident {
	class BuildSceneAssetProcess : public Process {
		private:
			/// Scene asset data layout:
			/// - Header
			/// 	- Signature
			/// 	- Identifier
			/// 	- Version
			/// 	- Static vertex count
			/// 	- Static index count
			/// 	- Dynamic vertex count
			/// 	- Dynamic index count
			/// 	- Entity count
			/// 	- Geometry count
			/// 	- Rigid body count
			/// - Vertex buffer (contains vertices for both static and dynamic geometry)
			/// - Index buffer (contains indices for both static and dynamic geometry)
			/// - Geometries
			/// 	- Centroid buffer
			/// 	- Radius buffer
			/// 	- Vertex offset buffer
			/// 	- Index offset buffer
			/// 	- Index count buffer
			/// 	- Type buffer
			/// - Rigid bodies
			/// 	- Position buffer
			/// 	- Scale buffer
			/// 	- Rotation buffer
			/// 	- Inverse mass buffer
			/// 	- Inverse inertia buffer
			/// 	- Friction buffer
			/// 	- Geometry index buffer
			/// - Geometry components
			static ByteStream<StreamOperation::WRITE> createTestScene2D(uint32 identifier) {
				std::vector<float32> dynamicVertexComponents;
				std::vector<uint16> dynamicIndices;

				const uint32 entityCount = 2;

				const sequence<CachedGeometry2D> geometries = {
					CachedGeometry2D::unitSquare(0, dynamicVertexComponents, dynamicIndices),
				};

				const sequence<GeometryComponent> geometryComponents = {
					{
						.geometryIndex = 0,
					},
					{
						.geometryIndex = 0,
					},
				};

				const sequence<CachedRigidBody2D> rigidBodies = {
					CachedRigidBody2D({
						.entity = 0,
						.position = {0.0_m, -10.0_m},
						.rotation = 0.0_rad,
						.scale = {100.0f, 20.0f},
						.mass = max<float32>(),
						.friction = 0.2f,
					}),
					CachedRigidBody2D({
						.entity = 1,
						.position = {0.0_m, 4.0_m},
						.rotation = 0.0_rad,
						.scale = {1.0f, 1.0f},
						.mass = 200.0_kg,
						.friction = 0.2f,
					}),
				};

				Scene2DAssetHeader header{};
				header.signature = AssetSignature::SCENE;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.staticVertexCount = 0;
				header.staticIndexCount = 0;
				header.dynamicVertexCount = static_cast<uint32>(dynamicVertexComponents.size()) / 2;
				header.dynamicIndexCount = static_cast<uint32>(dynamicIndices.size());
				header.entityCount = entityCount;
				header.geometryCount = static_cast<uint32>(geometries.count());
				header.rigidBodyCount = static_cast<uint32>(rigidBodies.count());
				header.geometryComponentCount = static_cast<uint32>(geometryComponents.count());

				uint64 size = 0;
				size += sizeof(header);
				size += header.staticVertexCount * sizeof(svec2<float32>);
				size += header.staticIndexCount * sizeof(uint16);
				size += header.dynamicVertexCount * sizeof(svec2<float32>);
				size += header.dynamicIndexCount * sizeof(uint16);
				size += header.geometryCount * (sizeof(Entity) + sizeof(svec2<float32>) + sizeof(float32) + sizeof(uint32) * 3 + sizeof(uint16) * 2 + sizeof(Geometry2DType));
				size += header.rigidBodyCount * (sizeof(Entity) + sizeof(svec2<float32>) * 2 + sizeof(float32) * 4);
				size += header.geometryComponentCount * sizeof(GeometryComponent);

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());
				stream.jump(sizeof(header));

				std::copy_n(pointer_cast<const uint8*>(&dynamicVertexComponents[0]), dynamicVertexComponents.size() * sizeof(float32), stream.getBytePosition());
				stream.jump(dynamicVertexComponents.size() * sizeof(float32));

				std::copy_n(pointer_cast<const uint8*>(&dynamicIndices[0]), dynamicIndices.size() * sizeof(uint16), stream.getBytePosition());
				stream.jump(dynamicIndices.size() * sizeof(uint16));

				// TODO(asset/scene): This is awful, review packing.
				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<svec2<float32>>(geometry.centroid);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<float32>(geometry.radius);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint32>(geometry.entity);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint32>(geometry.vertexOffset);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint32>(geometry.vertexCount);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint32>(geometry.indexOffset);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint16>(geometry.triangleListIndexCount);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint16>(geometry.lineStripIndexCount);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint8>(static_cast<uint8>(geometry.type));
				}

				for (const CachedRigidBody2D& rigidBody : rigidBodies) {
					stream.write<svec2<float32>>(rigidBody.position);
				}

				for (const CachedRigidBody2D& rigidBody : rigidBodies) {
					stream.write<svec2<float32>>(rigidBody.scale);
				}

				for (const CachedRigidBody2D& rigidBody : rigidBodies) {
					stream.write<float32>(rigidBody.rotation);
				}

				for (const CachedRigidBody2D& rigidBody : rigidBodies) {
					stream.write<float32>(rigidBody.inverseMass);
				}

				for (const CachedRigidBody2D& rigidBody : rigidBodies) {
					stream.write<float32>(rigidBody.inverseInertia);
				}

				for (const CachedRigidBody2D& rigidBody : rigidBodies) {
					stream.write<float32>(rigidBody.friction);
				}

				for (const CachedRigidBody2D& rigidBody : rigidBodies) {
					stream.write<uint32>(rigidBody.entity);
				}

				for (const GeometryComponent& geometryComponent : geometryComponents) {
					stream.write<GeometryComponent>(geometryComponent);
				}

				return stream;
			}

			static void encodeSceneAsset(const std::filesystem::path& assetOutputPath) {
				const view<char8> assetName = "Scene";
				const uint32 identifier = AssetLibrary::createAssetIdentifier();
				const ByteStream<StreamOperation::WRITE> stream = createTestScene2D(identifier);
				const uint64 size = stream.getByteOffset();
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, size);

				ENGINE_INFO("Generated scene asset \"[]\" (ID: []).", outputPath, identifier);
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = pointer_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";

				encodeSceneAsset(assetOutputPath);

				ENGINE_INFO("Done building scene assets.");

				return 0;
			}
	};
}