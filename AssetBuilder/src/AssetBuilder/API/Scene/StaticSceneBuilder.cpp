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

	StaticSceneBuilder::StaticSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName) :
		rootOutputPath(rootOutputPath),
		assetName(assetName),
		assetLibrary(new AssetLibrary(rootOutputPath))
	{}

	StaticSceneBuilder::~StaticSceneBuilder() {
		delete assetLibrary;
	}

	void StaticSceneBuilder::addMaterialShader(sequence<char8> name, ShaderStage stage, sequence<char8> entryPoint, sequence<DescriptorSetLayoutBinding> bindings) {
		materialShaders.emplace_back(BuildMaterialShader{
			.name = name,
			.stage = stage,
			.entryPoint = entryPoint,
			.descriptorSetLayoutBindings = bindings,
			.materialAssetRange = {0, 0},
		});
	}

	void StaticSceneBuilder::addMaterial(uint32 identifier, uint32 shaderIndex) {
		DEBUG_ASSERT(shaderIndex < materialShaders.size());

		materials.emplace_back(BuildStaticMaterial3D{
			.identifier = identifier,
			.shaderIndex = shaderIndex,
			.triangleRange = {0, 0},
		});

		materialShaders[shaderIndex].materialAssetRange.count += 1;
	}

	void StaticSceneBuilder::addSurface(const StaticSurface& surface) {
		ENGINE_ASSERT(surface.lightmapScale.x > 0.0f && surface.lightmapScale.y > 0.0f, "The lightmap scale must be positive.");

		surfaces.emplace_back(surface);
	}

	StaticScene3D StaticSceneBuilder::build() const {
		std::vector<svec3<float32>> vertices;
		std::vector<svec3<float32>> normals;
		std::vector<svec2<float32>> textureCoordinates;
		std::vector<StaticTriangle3D> triangles;

		const ref<GeometryAsset> geometryAsset = assetLibrary->getAsset<GeometryAsset>("UnitPlane");

		uint64 lightmapCount = 0;
		uint64 lightmapCoordinateCount = 0;

		for (const StaticSurface& surface : surfaces) {
			const view<Plane3D> faces = geometryAsset->getHull3DFaces();

			for (const Plane3D face : faces) {
				// One lightmap per face.
				lightmapCount += 1;

				// One coordinate per vertex on the face.
				lightmapCoordinateCount += face.edgeIndexRange.count;

				// TODO(asset-builder/lightmapper): Handle UnitPlane back face.
				if (faces.count() == 2) {
					break;
				}
			}
		}

		// Sorted by lightmap area (ascending).
		std::vector<LightmapIndex> lightmapIndices;
		lightmapIndices.reserve(lightmapCount);

		sequence<Lightmap> lightmaps(lightmapCount);

		StaticScene3D scene{};
		scene.lightmapCoordinates = sequence<svec2<float32>>(lightmapCoordinateCount);
		scene.materialShaders = sequence<BuildMaterialShader>(materialShaders.size());
		scene.materials = sequence<BuildStaticMaterial3D>(materials.size());

		std::copy(materialShaders.begin(), materialShaders.end(), &scene.materialShaders[0]);
		std::copy(materials.begin(), materials.end(), &scene.materials[0]);

		uint32 faceIndex = 0;
		uint32 lightmapCoordinateOffset = 0;

		for (const StaticSurface& surface : surfaces) {
			const view<float32> vertexPositionComponents = geometryAsset->getVertexPositionComponents();
			const view<float32> vertexNormalComponents = geometryAsset->getVertexNormalComponents();
			const view<float32> vertexTextureCoordinateComponents = geometryAsset->getVertexTextureCoordinateComponents();
			const uint64 vertexCount = vertexPositionComponents.count() / 3;
			const uint64 normalCount = vertexNormalComponents.count() / 3;
			const uint64 textureCoordinateCount = vertexTextureCoordinateComponents.count() / 2;
			const vec3<float32> scale = {surface.transform.scale[0], 0.0f, surface.transform.scale[1]};
			const quat<float32> pitch = quat<float32>::axisAngle(RIGHT, surface.transform.rotation[0]);
			const quat<float32> yaw = quat<float32>::axisAngle(UP, surface.transform.rotation[1]);
			const quat<float32> roll = quat<float32>::axisAngle(FORWARD, surface.transform.rotation[2]);
			const quat<float32> orientation = normalized(yaw * pitch * roll);
			const mat4x4<float32> world = (
				mat4x4<float32>::scaling(scale) *
				mat4x4<float32>(orientation) *
				mat4x4<float32>::translation(surface.transform.position)
			);

			BuildStaticMaterial3D& material = scene.materials[surface.materialIndex];

			sequence<uint32> vertexRedirectionIndices(vertexCount);
			sequence<uint32> normalRedirectionIndices(normalCount);
			sequence<uint32> textureCoordinateRedirectionIndices(textureCoordinateCount);

			// Merge vertex duplicates.
			{
				uint32 vertexIndex = 0;

				while (vertexIndex < vertexCount) {
					vec3<float32> v = {
						vertexPositionComponents[vertexIndex * 3 + 0],
						vertexPositionComponents[vertexIndex * 3 + 1],
						vertexPositionComponents[vertexIndex * 3 + 2],
					};
					v = multiplyTransform(v, world);

					const svec3<float32> vertex = {
						round(v[0] * 100.0f) / 100.0f,
						round(v[1] * 100.0f) / 100.0f,
						round(v[2] * 100.0f) / 100.0f,
					};

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
					vec3<float32> n = {
						vertexNormalComponents[normalIndex * 3 + 0],
						vertexNormalComponents[normalIndex * 3 + 1],
						vertexNormalComponents[normalIndex * 3 + 2],
					};
					n = normalized(n * inverse(orientation));

					const svec3<float32> normal(n);

					view<svec3<float32>> normalView{};

					if (normals.size() > 0) {
						normalView = view<svec3<float32>>(&normals[0], normals.size());
					}

					const uint32 normalRedirectionIndex = findDuplicate(normalView, normal);

					if (normalRedirectionIndex >= normals.size()) {
						// TODO(asset-builder/lightmapper): Don't add every vertex normal if only the face normals are used.
						normals.emplace_back(normal);
					}

					normalRedirectionIndices[normalIndex] = normalRedirectionIndex;

					normalIndex += 1;
				}
			}

			const mat3x3<float32> textureWorld = (
				mat3x3<float32>::scaling(surface.transform.scale * vec2<float32>(surface.textureFiltering.scale)) *
				mat3x3<float32>::rotation(pi<float32>() * surface.textureFiltering.rotation) *
				mat3x3<float32>::translation(surface.textureFiltering.translation)
			);

			// Merge texture coordinate duplicates.
			{
				uint32 textureCoordinateIndex = 0;

				while (textureCoordinateIndex < textureCoordinateCount) {
					const svec2<float32> textureCoordinate = svec2<float32>(multiplyTransform({
						vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 0],
						vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 1],
					}, textureWorld));

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

			const view<uint32> vertexPositionIndices = geometryAsset->getVertexPositionIndices();
			const view<uint32> vertexNormalIndices = geometryAsset->getVertexNormalIndices();
			const view<uint32> vertexTextureCoordinateIndices = geometryAsset->getVertexTextureCoordinateIndices();

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

				material.triangleRange.count += 1;
				indexOffset += 3;
			}

			if (surface.materialIndex > 0) {
				const BuildStaticMaterial3D& previousMaterial = scene.materials[surface.materialIndex - 1];

				material.triangleRange.offset = previousMaterial.triangleRange.offset + previousMaterial.triangleRange.count;
			}

			const view<Edge3D> edges = geometryAsset->getHull3DEdges();
			const view<Plane3D> faces = geometryAsset->getHull3DFaces();
			const view<uint16> faceEdgeIndices = geometryAsset->getHull3DFaceEdgeIndices();

			for (const Plane3D face : faces) {
				const uint32 lightmapCoordinateCount = face.edgeIndexRange.count;

				Lightmap& lightmap = lightmaps[faceIndex];
				lightmap.edgeIndexRange = face.edgeIndexRange;
				lightmap.offset = {0, 0};
				lightmap.coordinateOffset = lightmapCoordinateOffset;

				svec2<float32> maxCoordinate = {min<float32>(), min<float32>()};
				svec2<float32> minCoordinate = {max<float32>(), max<float32>()};
				uint16 coordinateIndex = 0;
				quat<float32> R;

				// TODO(asset-builder/lightmapper)
				{
					vec3<float32> normal(face.normal);
					normal = normalized(normal * orientation);

					const float32 projection = dot(normal, UP);

					if (projection > 0.999999f) {
						R = quat<float32>::identity();
					}
					else if (projection < -0.999999f) {
						vec3<float32> axis = normalized(cross(normal, RIGHT));

						if (length(axis) < 0.000001f) {
							axis = normalized(cross(normal, FORWARD));
						}

						R = quat<float32>::axisAngle(axis, pi<float32>());
					}
					else {
						const vec3<float32> axis = cross(normal, UP);

						R = {
							projection + 1.0f,
							axis[0],
							axis[1],
							axis[2],
						};
						R = normalized(R);
					}
				}

				while (coordinateIndex < lightmapCoordinateCount) {
					const uint16 edgeIndex = faceEdgeIndices[face.edgeIndexRange.offset + coordinateIndex];
					const Edge3D edge = edges[edgeIndex];
					const uint32 vertexPositionComponentOffset = edge.originVertexIndex * 3;

					vec3<float32> vertex = {
						vertexPositionComponents[vertexPositionComponentOffset + 0],
						vertexPositionComponents[vertexPositionComponentOffset + 1],
						vertexPositionComponents[vertexPositionComponentOffset + 2],
					};
					vertex *= scale;
					vertex *= orientation;
					vertex *= R;

					svec2<float32>& coordinate = scene.lightmapCoordinates[lightmapCoordinateOffset + edge.originVertexIndex];
					coordinate = {vertex[0], vertex[2]};
					coordinate /= surface.lightmapScale;
					coordinate.x = round(coordinate.x * 100.0f) / 100.0f;
					coordinate.y = round(coordinate.y * 100.0f) / 100.0f;

					maxCoordinate.x = max(maxCoordinate.x, coordinate.x);
					maxCoordinate.y = max(maxCoordinate.y, coordinate.y);
					minCoordinate.x = min(minCoordinate.x, coordinate.x);
					minCoordinate.y = min(minCoordinate.y, coordinate.y);

					coordinateIndex += 1;
				}

				DEBUG_ASSERT(maxCoordinate.x > 0.0f);
				DEBUG_ASSERT(maxCoordinate.y > 0.0f);

				coordinateIndex = 0;

				while (coordinateIndex < lightmapCoordinateCount) {
					svec2<float32>& coordinate = scene.lightmapCoordinates[lightmapCoordinateOffset + coordinateIndex];
					coordinate.y *= -1.0f;
					coordinate += maxCoordinate;

					coordinateIndex += 1;
				}

				// Pad for dilation.
				maxCoordinate.x += 2.0f;
				maxCoordinate.y += 2.0f;

				const svec2<float32> size = abs(maxCoordinate - minCoordinate);

				lightmap.size = {static_cast<uint32>(size.x), static_cast<uint32>(size.y)};

				LightmapIndex lightmapIndex{};
				lightmapIndex.index = faceIndex;
				lightmapIndex.area = lightmap.size.width * lightmap.size.height;

				uint32 lightmapInsertionIndex = 0;

				while (lightmapInsertionIndex < lightmapIndices.size()) {
					if (lightmapIndex.area > lightmapIndices[lightmapInsertionIndex].area) {
						break;
					}

					lightmapInsertionIndex += 1;
				}

				lightmapIndices.insert(lightmapIndices.begin() + lightmapInsertionIndex, lightmapIndex);

				lightmapCoordinateOffset += lightmapCoordinateCount;
				faceIndex += 1;

				// TODO(asset-builder/lightmapper): Handle UnitPlane back face.
				if (faces.count() == 2) {
					break;
				}
			}
		}

		scene.vertices = sequence<svec3<float32>>(vertices.size());
		scene.normals = sequence<svec3<float32>>(normals.size());
		scene.textureCoordinates = sequence<svec2<float32>>(textureCoordinates.size());
		scene.triangles = sequence<StaticTriangle3D>(triangles.size());

		std::copy(vertices.begin(), vertices.end(), &scene.vertices[0]);
		std::copy(normals.begin(), normals.end(), &scene.normals[0]);
		std::copy(textureCoordinates.begin(), textureCoordinates.end(), &scene.textureCoordinates[0]);
		std::copy(triangles.begin(), triangles.end(), &scene.triangles[0]);

		Lightmapper::updateAtlasCoordinates(scene, lightmaps, view<LightmapIndex>(&lightmapIndices[0], lightmapIndices.size()), geometryAsset);

		ImageAssetDescriptor lightmapAtlasImageAssetDescriptor = {
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {Lightmapper::ATLAS_SIZE.width, Lightmapper::ATLAS_SIZE.width, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.data = sequence<uint8>(Lightmapper::ATLAS_SIZE.width * Lightmapper::ATLAS_SIZE.height * getByteSize(ImageFormat::R8G8B8A8_UNORM)),
		};

		Lightmapper lightmapper;
		lightmapper.buildLightmapAtlas(scene, lightmapAtlasImageAssetDescriptor);

		BuildImageAssetProcess::encodeImageAsset(rootOutputPath / "Asset/", assetName, lightmapAtlasImageAssetDescriptor);

		scene.lightmapAtlasImageAssetIdentifier = lightmapAtlasImageAssetDescriptor.identifier;

		return scene;
	}
}