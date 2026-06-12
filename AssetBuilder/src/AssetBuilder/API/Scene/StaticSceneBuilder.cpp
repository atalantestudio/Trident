// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Lightmap.hpp"
#include "AssetBuilder/API/Scene/Lightmapper.hpp"
#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

#include "../../../File/PNG.hpp"
#include "../../../Process/BuildImageAssetProcess.hpp"

namespace Trident {
	static bool compareDuplicate(svec2<float32> a, svec2<float32> b, float32 E) {
		return (
			abs(a[0] - b[0]) < E &&
			abs(a[1] - b[1]) < E
		);
	}

	static bool compareDuplicate(svec3<float32> a, svec3<float32> b, float32 E) {
		return (
			abs(a[0] - b[0]) < E &&
			abs(a[1] - b[1]) < E &&
			abs(a[2] - b[2]) < E
		);
	}

	template<typename T>
	static uint32 findDuplicate(view<T> list, T a) {
		constexpr float32 E = 0.001f;

		uint32 index = 0;

		while (index < list.count()) {
			const T b = list[index];

			if (compareDuplicate(a, b, E)) {
				break;
			}

			index += 1;
		}

		return index;
	}

	StaticSceneBuilder::StaticSceneBuilder(std::filesystem::path rootOutputPath) :
		rootOutputPath(rootOutputPath),
		assetLibrary(new AssetLibrary(rootOutputPath))
	{}

	StaticSceneBuilder::~StaticSceneBuilder() {
		delete assetLibrary;
	}

	StaticSceneBuilder& StaticSceneBuilder::allocateMaterials(uint32 count) {
		DEBUG_ASSERT(count > 0);

		materials = sequence<StaticMaterial>(count);

		return *this;
	}

	StaticSceneBuilder& StaticSceneBuilder::insertMaterial(uint32 index, const StaticMaterial& material) {
		DEBUG_ASSERT(index < materials.count());

		materials[index] = material;

		return *this;
	}

	StaticSceneBuilder& StaticSceneBuilder::createStaticObject(const StaticObjectDescriptor& descriptor) {
		ENGINE_ASSERT(descriptor.lightmapPixelScale.x > 0.0f && descriptor.lightmapPixelScale.y > 0.0f, "The lightmap pixel scale must be positive.");

		objects.emplace_back(StaticObject{
			.geometryAsset = assetLibrary->getAsset<GeometryAsset>(descriptor.geometryAssetName),
			.transform = descriptor.transform,
			.texture = (
				mat3x3<float32>::scaling(descriptor.textureFiltering.scale) *
				mat3x3<float32>::rotation(descriptor.textureFiltering.rotation) *
				mat3x3<float32>::translation(descriptor.textureFiltering.translation)
			),
			.lightmapPixelScale = descriptor.lightmapPixelScale,
		});

		return *this;
	}

	StaticScene3D StaticSceneBuilder::build() const {
		std::vector<StaticTriangle3D> triangles;
		std::vector<svec3<float32>> vertices;
		std::vector<svec3<float32>> normals;
		std::vector<svec2<float32>> textureCoordinates;

		uint64 lightmapCount = 0;
		uint64 lightmapCoordinateCount = 0;

		for (const StaticObject& object : objects) {
			const view<Plane3D> faces = object.geometryAsset->getHull3DFaces();

			for (const Plane3D face : faces) {
				// One lightmap per face.
				lightmapCount += 1;

				// One coordinate per vertex on the face.
				lightmapCoordinateCount += face.edgeIndexRange.count;

				// TODO: Ignore other faces for now.
				break;
			}
		}

		std::vector<Lightmap> lightmaps;
		lightmaps.reserve(lightmapCount);

		StaticScene3D scene{};
		scene.lightmapCoordinates = sequence<svec2<float32>>(lightmapCoordinateCount);

		uint32 lightmapCoordinateOffset = 0;

		for (const StaticObject& object : objects) {
			const view<float32> vertexPositionComponents = object.geometryAsset->getVertexPositionComponents();
			const view<float32> vertexNormalComponents = object.geometryAsset->getVertexNormalComponents();
			const view<float32> vertexTextureCoordinateComponents = object.geometryAsset->getVertexTextureCoordinateComponents();
			const uint64 vertexCount = vertexPositionComponents.count() / 3;
			const uint64 normalCount = vertexNormalComponents.count() / 3;
			const uint64 textureCoordinateCount = vertexTextureCoordinateComponents.count() / 2;
			const quat<float32> pitch = quat<float32>::axisAngle(RIGHT, object.transform.rotation[0]);
			const quat<float32> yaw = quat<float32>::axisAngle(UP, object.transform.rotation[1]);
			const quat<float32> roll = quat<float32>::axisAngle(FORWARD, object.transform.rotation[2]);
			const quat<float32> orientation = normalized(yaw * pitch * roll);
			const mat4x4<float32> world = (
				mat4x4<float32>(orientation) *
				mat4x4<float32>::scaling(object.transform.scale) *
				mat4x4<float32>::translation(object.transform.position)
			);

			sequence<uint32> vertexRedirectionIndices(vertexCount);
			sequence<uint32> normalRedirectionIndices(normalCount);
			sequence<uint32> textureCoordinateRedirectionIndices(textureCoordinateCount);

			// Merge vertex duplicates.
			{
				uint32 vertexIndex = 0;

				while (vertexIndex < vertexCount) {
					svec3<float32> vertex = svec3<float32>(multiplyTransform({
						vertexPositionComponents[vertexIndex * 3 + 0],
						vertexPositionComponents[vertexIndex * 3 + 1],
						vertexPositionComponents[vertexIndex * 3 + 2],
					}, world));

					uint32 componentIndex = 0;

					while (componentIndex < 3) {
						if (abs(fract(vertex[componentIndex])) < 0.0001f) {
							vertex[componentIndex] = round(vertex[componentIndex]);
						}

						componentIndex += 1;
					}

					view<svec3<float32>> vertexView{};

					if (vertices.size() > 0) {
						vertexView = view<svec3<float32>>(&vertices[0], vertices.size());
					}

					const uint32 vertexRedirectionIndex = findDuplicate(vertexView, vertex);

					if (vertexRedirectionIndex >= vertices.size()) {
						vertices.emplace_back(vertex);
					}

					vertexRedirectionIndices[vertexIndex] = vertexRedirectionIndex;

					vertexIndex += 1;
				}
			}

			// Merge normal duplicates.
			{
				uint32 normalIndex = 0;

				while (normalIndex < normalCount) {
					const svec3<float32> normal = {
						vertexNormalComponents[normalIndex * 3 + 0],
						vertexNormalComponents[normalIndex * 3 + 1],
						vertexNormalComponents[normalIndex * 3 + 2],
					};

					view<svec3<float32>> normalView{};

					if (normals.size() > 0) {
						normalView = view<svec3<float32>>(&normals[0], normals.size());
					}

					const uint32 normalRedirectionIndex = findDuplicate(normalView, normal);

					if (normalRedirectionIndex >= normals.size()) {
						// TODO: Don't add every vertex normal if only the face normals are used.
						normals.emplace_back(normal);
					}

					normalRedirectionIndices[normalIndex] = normalRedirectionIndex;

					normalIndex += 1;
				}
			}

			// Merge texture coordinate duplicates.
			{
				uint32 textureCoordinateIndex = 0;

				while (textureCoordinateIndex < textureCoordinateCount) {
					const svec2<float32> textureCoordinate = svec2<float32>(multiplyTransform({
						vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 0],
						vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 1],
					}, object.texture));

					view<svec2<float32>> textureCoordinateView{};

					if (textureCoordinates.size() > 0) {
						textureCoordinateView = view<svec2<float32>>(&textureCoordinates[0], textureCoordinates.size());
					}

					const uint32 textureCoordinateRedirectionIndex = findDuplicate(textureCoordinateView, textureCoordinate);

					if (textureCoordinateRedirectionIndex >= textureCoordinates.size()) {
						textureCoordinates.emplace_back(textureCoordinate);
					}

					textureCoordinateRedirectionIndices[textureCoordinateIndex] = textureCoordinateRedirectionIndex;

					textureCoordinateIndex += 1;
				}
			}

			const view<uint32> vertexPositionIndices = object.geometryAsset->getVertexPositionIndices();
			const view<uint32> vertexNormalIndices = object.geometryAsset->getVertexNormalIndices();
			const view<uint32> vertexTextureCoordinateIndices = object.geometryAsset->getVertexTextureCoordinateIndices();

			uint32 indexOffset = 0;

			while (indexOffset < vertexPositionIndices.count()) {
				const uint32 normalIndex = vertexNormalIndices[indexOffset];

				ENGINE_ASSERT(
					normalIndex == vertexNormalIndices[indexOffset + 1] &&
					normalIndex == vertexNormalIndices[indexOffset + 2],
					"Static scenes must use face normals."
				);

				triangles.emplace_back(StaticTriangle3D{
					.vertices = {
						{
							.vertexIndex = vertexRedirectionIndices[vertexPositionIndices[indexOffset + 0]],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 0]],
							.lightmapCoordinateIndex = 0,
						},
						{
							.vertexIndex = vertexRedirectionIndices[vertexPositionIndices[indexOffset + 1]],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 1]],
							.lightmapCoordinateIndex = 0,
						},
						{
							.vertexIndex = vertexRedirectionIndices[vertexPositionIndices[indexOffset + 2]],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 2]],
							.lightmapCoordinateIndex = 0,
						},
					},
					.normalIndex = normalRedirectionIndices[normalIndex],
				});

				indexOffset += 3;
			}

			const view<Edge3D> edges = object.geometryAsset->getHull3DEdges();
			const view<Plane3D> faces = object.geometryAsset->getHull3DFaces();
			const view<uint16> faceEdgeIndices = object.geometryAsset->getHull3DFaceEdgeIndices();

			const vec3<float32> T = object.transform.scale * inverse(orientation) * quat<float32>::axisAngle({0.0f, 1.0f, 0.0f}, 0.0f);

			for (const Plane3D face : faces) {
				const uint32 lightmapCoordinateCount = face.edgeIndexRange.count;

				Lightmap lightmap{};
				lightmap.geometryAsset = object.geometryAsset;
				lightmap.edgeIndexRange = face.edgeIndexRange;
				lightmap.offset = {0, 0};
				lightmap.objectScale = object.transform.scale;
				lightmap.pixelScale = object.lightmapPixelScale;

				svec2<float32> maxCoordinate = {min<float32>(), min<float32>()};
				svec2<float32> minCoordinate = {max<float32>(), max<float32>()};
				uint16 coordinateIndex = 0;

				while (coordinateIndex < lightmapCoordinateCount) {
					const uint16 edgeIndex = faceEdgeIndices[face.edgeIndexRange.offset + coordinateIndex];
					const Edge3D edge = edges[edgeIndex];
					const uint32 vertexPositionComponentOffset = edge.originVertexIndex * 3;

					vec3<float32> vertex = {
						vertexPositionComponents[vertexPositionComponentOffset + 0],
						vertexPositionComponents[vertexPositionComponentOffset + 1],
						vertexPositionComponents[vertexPositionComponentOffset + 2],
					};
					vertex *= T;

					svec2<float32>& coordinate = scene.lightmapCoordinates[lightmapCoordinateOffset + edge.originVertexIndex];
					coordinate = {vertex[0], vertex[2]};
					coordinate /= lightmap.pixelScale;

					if (abs(fract(coordinate.x)) < 0.0001f || abs(fract(coordinate.x)) > 0.9999f) {
						coordinate.x = round(coordinate.x);
					}

					if (abs(fract(coordinate.y)) < 0.0001f || abs(fract(coordinate.y)) > 0.9999f) {
						coordinate.y = round(coordinate.y);
					}

					maxCoordinate.x = max(maxCoordinate.x, coordinate.x);
					maxCoordinate.y = max(maxCoordinate.y, coordinate.y);
					minCoordinate.x = min(minCoordinate.x, coordinate.x);
					minCoordinate.y = min(minCoordinate.y, coordinate.y);

					coordinateIndex += 1;
				}

				coordinateIndex = 0;

				while (coordinateIndex < lightmapCoordinateCount) {
					svec2<float32>& coordinate = scene.lightmapCoordinates[lightmapCoordinateOffset + coordinateIndex];
					coordinate.y *= -1.0f;
					coordinate += maxCoordinate;

					coordinateIndex += 1;
				}

				const svec2<float32> size = abs(maxCoordinate - minCoordinate);

				lightmap.size = {static_cast<uint32>(size.x), static_cast<uint32>(size.y)};
				lightmap.area = lightmap.size.width * lightmap.size.height;

				uint32 lightmapInsertionIndex = 0;

				while (lightmapInsertionIndex < lightmaps.size()) {
					if (lightmap.area > lightmaps[lightmapInsertionIndex].area) {
						break;
					}

					lightmapInsertionIndex += 1;
				}

				lightmaps.insert(lightmaps.begin() + lightmapInsertionIndex, lightmap);

				lightmapCoordinateOffset += lightmapCoordinateCount;

				// TODO: Ignore other faces for now.
				break;
			}
		}

		scene.triangles = sequence<StaticTriangle3D>(triangles.size());
		scene.vertices = sequence<svec3<float32>>(vertices.size());
		scene.normals = sequence<svec3<float32>>(normals.size());
		scene.textureCoordinates = sequence<svec2<float32>>(textureCoordinates.size());

		std::copy(triangles.begin(), triangles.end(), &scene.triangles[0]);
		std::copy(vertices.begin(), vertices.end(), &scene.vertices[0]);
		std::copy(normals.begin(), normals.end(), &scene.normals[0]);
		std::copy(textureCoordinates.begin(), textureCoordinates.end(), &scene.textureCoordinates[0]);

		Lightmapper::updateAtlasCoordinates(lightmaps, scene);

		ImageAssetDescriptor lightmapAtlasImageAssetDescriptor{};
		lightmapAtlasImageAssetDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		lightmapAtlasImageAssetDescriptor.extent = {Lightmapper::ATLAS_SIZE.width, Lightmapper::ATLAS_SIZE.width, 1};
		lightmapAtlasImageAssetDescriptor.mipLevelCount = 1;
		lightmapAtlasImageAssetDescriptor.layerCount = 1;
		lightmapAtlasImageAssetDescriptor.data = sequence<uint8>(lightmapAtlasImageAssetDescriptor.layerCount * lightmapAtlasImageAssetDescriptor.extent.width * lightmapAtlasImageAssetDescriptor.extent.height * lightmapAtlasImageAssetDescriptor.extent.depth * getChannelCount(lightmapAtlasImageAssetDescriptor.format));

		Lightmapper lightmapper;
		lightmapper.buildLightmapAtlas(scene, lightmapAtlasImageAssetDescriptor);

		BuildImageAssetProcess::encodeImageAsset(rootOutputPath / "Asset/", "test_lightmap", lightmapAtlasImageAssetDescriptor);

		return scene;
	}
}