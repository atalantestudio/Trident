// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Lightmap.hpp"
#include "AssetBuilder/API/Scene/Lightmapper.hpp"
#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"

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
		assetLibrary(new AssetLibrary(rootOutputPath))
	{}

	StaticSceneBuilder::~StaticSceneBuilder() {
		delete assetLibrary;
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
		std::vector<Lightmap> lightmaps;

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

			static const quat<float32> up = quat<float32>::axisAngle({0.0f, 1.0f, 0.0f}, 0.0f);

			for (const Plane3D face : faces) {
				Lightmap lightmap{};
				svec2<float32> maxCoordinate = {min<float32>(), min<float32>()};
				svec2<float32> minCoordinate = {max<float32>(), max<float32>()};
				uint16 edgeIndexIndex = 0;

				while (edgeIndexIndex < face.edgeIndexRange.count) {
					const uint16 edgeIndex = faceEdgeIndices[face.edgeIndexRange.offset + edgeIndexIndex];
					const Edge3D edge = edges[edgeIndex];

					const vec3<float32> a = (vec3<float32>{
						vertexPositionComponents[edge.originVertexIndex * 3 + 0],
						vertexPositionComponents[edge.originVertexIndex * 3 + 1],
						vertexPositionComponents[edge.originVertexIndex * 3 + 2],
					} * object.transform.scale) * up;

					const svec2<float32> lightmapCoordinate = svec2<float32>{a[0], a[2]} / object.lightmapPixelScale;

					lightmap.coordinates.emplace_back(lightmapCoordinate);

					maxCoordinate.x = max(maxCoordinate.x, lightmapCoordinate.x);
					maxCoordinate.y = max(maxCoordinate.y, lightmapCoordinate.y);
					minCoordinate.x = min(minCoordinate.x, lightmapCoordinate.x);
					minCoordinate.y = min(minCoordinate.y, lightmapCoordinate.y);

					edgeIndexIndex += 1;
				}

				lightmap.offset = {0.0f, 0.0f};
				lightmap.size = abs(maxCoordinate - minCoordinate);
				lightmap.area = lightmap.size.x * lightmap.size.y;

				uint32 lightmapIndex = 0;

				while (lightmapIndex < lightmaps.size()) {
					if (lightmap.area > lightmaps[lightmapIndex].area) {
						break;
					}

					lightmapIndex += 1;
				}

				lightmaps.insert(lightmaps.begin() + lightmapIndex, lightmap);
			}
		}

		// TODO
		// 1. Insert each rectangle into a 2048x2048 texture array using this packing algorithm: https://blackpawn.com/texts/lightmaps.
		// 2. After inserting a rectangle, calculate store its final UVs into the static triangles composing the lightmapped face.

		StaticScene3D staticScene{};
		staticScene.triangles = sequence<StaticTriangle3D>(triangles.size());
		staticScene.vertices = sequence<svec3<float32>>(vertices.size());
		staticScene.normals = sequence<svec3<float32>>(normals.size());
		staticScene.textureCoordinates = sequence<svec2<float32>>(textureCoordinates.size());

		std::copy(triangles.begin(), triangles.end(), &staticScene.triangles[0]);
		std::copy(vertices.begin(), vertices.end(), &staticScene.vertices[0]);
		std::copy(normals.begin(), normals.end(), &staticScene.normals[0]);
		std::copy(textureCoordinates.begin(), textureCoordinates.end(), &staticScene.textureCoordinates[0]);

		Lightmapper::updateAtlasCoordinates(view<Lightmap>(&lightmaps[0], lightmaps.size()), staticScene.triangles);

		return staticScene;
	}
}