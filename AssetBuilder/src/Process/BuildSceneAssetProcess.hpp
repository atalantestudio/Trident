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
			/// 
			/// Scene asset data layout:
			/// - Header
			/// 	- Signature
			/// 	- Identifier
			/// 	- Version
			/// 	- Static vertex count
			/// 	- Static index count
			/// 	- Dynamic vertex count
			/// 	- Dynamic index count
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
			/// 
			static ByteStream<StreamOperation::WRITE> createTestScene2D(uint32 identifier) {
				const sequence<float32> dynamicVertices = {
					-0.5f,  0.5f,
					 0.5f,  0.5f,
					 0.5f, -0.5f,
					-0.5f, -0.5f,
				};

				const sequence<uint16> dynamicIndices = {
					0, 1, 2,
					0, 2, 3,
				};

				const sequence<CachedGeometry2D> geometries = {
					CachedGeometry2D(ConvexHullGeometry2D::unitSquare(), 0, 0),
				};

				const sequence<CachedRigidBody2D> rigidBodies = {
					CachedRigidBody2D({
						.position = {0.0_m, -10.0_m},
						.rotation = 0.0_rad,
						.scale = {100.0f, 20.0f},
						.mass = 1.0_kg,
						.friction = 0.2f,
						.geometryIndex = 0,
					}),
				};

				Scene2DAssetHeader header{};
				header.signature = AssetSignature::SCENE;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.staticVertexCount = 0;
				header.staticIndexCount = 0;
				header.dynamicVertexCount = static_cast<uint32>(dynamicVertices.count()) / 2;
				header.dynamicIndexCount = static_cast<uint32>(dynamicIndices.count());
				header.geometryCount = static_cast<uint32>(geometries.count());
				header.rigidBodyCount = static_cast<uint32>(rigidBodies.count());

				uint64 size = 0;
				size += sizeof(header);
				size += header.staticVertexCount * sizeof(svec2<float32>);
				size += header.staticIndexCount * sizeof(uint16);
				size += header.dynamicVertexCount * sizeof(svec2<float32>);
				size += header.dynamicIndexCount * sizeof(uint16);
				size += header.geometryCount * (sizeof(svec2<float32>) + sizeof(float32) + sizeof(uint32) * 2 + sizeof(uint16) + sizeof(uint8));
				size += header.rigidBodyCount * (sizeof(svec2<float32>) * 2 + sizeof(float32) * 4 + sizeof(uint32));

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());
				stream.jump(sizeof(header));

				std::copy_n(pointer_cast<const uint8*>(&dynamicVertices[0]), dynamicVertices.count() * sizeof(float32), stream.getBytePosition());
				stream.jump(dynamicVertices.count() * sizeof(float32));

				std::copy_n(pointer_cast<const uint8*>(&dynamicIndices[0]), dynamicIndices.count() * sizeof(uint16), stream.getBytePosition());
				stream.jump(dynamicIndices.count() * sizeof(uint16));

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<svec2<float32>>(geometry.centroid);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<float32>(geometry.radius);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint32>(geometry.vertexOffset);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint32>(geometry.indexOffset);
				}

				for (const CachedGeometry2D& geometry : geometries) {
					stream.write<uint16>(geometry.indexCount);
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
					stream.write<uint32>(rigidBody.geometryIndex);
				}

				return stream;
			}

			static void encodeSceneAsset(const std::filesystem::path& assetOutputPath) {
				const uint32 identifier = AssetLibrary::createAssetIdentifier();
				const ByteStream<StreamOperation::WRITE> stream = createTestScene2D(identifier);
				const uint64 size = stream.getByteOffset();
				const std::string assetOutputPathString = assetOutputPath.string();

				view<char8> assetName = "Scene";

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