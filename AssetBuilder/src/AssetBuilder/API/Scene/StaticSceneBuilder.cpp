// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

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
		objects.emplace_back(StaticObject{
			.geometryAsset = assetLibrary->getAsset<GeometryAsset>(descriptor.geometryAssetName),
			.transform = descriptor.transform,
			.texture = (
				mat3x3<float32>::scaling(descriptor.textureFiltering.scale) *
				mat3x3<float32>::rotation(descriptor.textureFiltering.rotation) *
				mat3x3<float32>::translation(descriptor.textureFiltering.translation)
			),
		});

		return *this;
	}

	StaticScene3D StaticSceneBuilder::build() const {
		std::vector<StaticTriangle3D> triangles;
		std::vector<svec3<float32>> vertices;
		std::vector<svec3<float32>> normals;
		std::vector<svec2<float32>> textureCoordinates;

		for (const StaticObject& object : objects) {
			const view<float32> vertexPositionComponents = object.geometryAsset->getVertexPositionComponents();
			const view<float32> vertexNormalComponents = object.geometryAsset->getVertexNormalComponents();
			const view<float32> vertexTextureCoordinateComponents = object.geometryAsset->getVertexTextureCoordinateComponents();
			const uint64 vertexCount = vertexPositionComponents.count() / 3;
			const uint64 normalCount = vertexNormalComponents.count() / 3;
			const uint64 textureCoordinateCount = vertexTextureCoordinateComponents.count() / 2;
			const mat4x4<float32> world = (
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
					const svec3<float32> vertex = svec3<float32>(multiplyTransform({
						vertexPositionComponents[vertexIndex * 3 + 0],
						vertexPositionComponents[vertexIndex * 3 + 1],
						vertexPositionComponents[vertexIndex * 3 + 2],
					}, world));

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
					.aIndex = vertexRedirectionIndices[vertexPositionIndices[indexOffset + 0]],
					.bIndex = vertexRedirectionIndices[vertexPositionIndices[indexOffset + 1]],
					.cIndex = vertexRedirectionIndices[vertexPositionIndices[indexOffset + 2]],
					.normalIndex = normalRedirectionIndices[normalIndex],
					.aUvIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 0]],
					.bUvIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 1]],
					.cUvIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 2]],
				});

				indexOffset += 3;
			}
		}

		StaticScene3D staticScene{};
		staticScene.triangles = sequence<StaticTriangle3D>(triangles.size());
		staticScene.vertices = sequence<svec3<float32>>(vertices.size());
		staticScene.normals = sequence<svec3<float32>>(normals.size());
		staticScene.textureCoordinates = sequence<svec2<float32>>(textureCoordinates.size());

		std::copy(triangles.begin(), triangles.end(), &staticScene.triangles[0]);
		std::copy(vertices.begin(), vertices.end(), &staticScene.vertices[0]);
		std::copy(normals.begin(), normals.end(), &staticScene.normals[0]);
		std::copy(textureCoordinates.begin(), textureCoordinates.end(), &staticScene.textureCoordinates[0]);

		return staticScene;
	}
}