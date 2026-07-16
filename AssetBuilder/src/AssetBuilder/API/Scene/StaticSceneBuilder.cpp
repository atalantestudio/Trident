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
		assetLibrary(new AssetLibrary(rootOutputPath)),
		surfaceGeometry(assetLibrary->getAsset<GeometryAsset>("UnitPlane"))
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
		sequence<StaticSurface> surfaces(this->surfaces.size());

		std::copy(this->surfaces.begin(), this->surfaces.end(), &surfaces[0]);

		const Plane3D& face = surfaceGeometry->getHull3DFaces()[0];
		const uint32 quadCount = carveSurfaces(surfaces);
		const uint32 triangleCount = quadCount * 2;
		const uint32 lightmapCoordinateCount = quadCount * 4;

		StaticScene3D scene{};
		scene.lightmapCoordinates = sequence<svec2<float32>>(lightmapCoordinateCount);
		scene.materialShaders = sequence<BuildMaterialShader>(materialShaders.size());
		scene.materials = sequence<BuildStaticMaterial3D>(materials.size());
		scene.triangles = sequence<StaticTriangle3D>(triangleCount);

		std::vector<svec3<float32>> vertices;
		std::vector<svec3<float32>> normals;
		std::vector<svec2<float32>> textureCoordinates;
		sequence<Lightmap> lightmaps(quadCount);

		// Sorted by ascending lightmap area.
		std::vector<LightmapIndex> lightmapIndices;
		lightmapIndices.reserve(quadCount);

		std::copy(materialShaders.begin(), materialShaders.end(), &scene.materialShaders[0]);
		std::copy(materials.begin(), materials.end(), &scene.materials[0]);

		uint32 lightmapCoordinateOffset = 0;
		uint32 triangleIndex = 0;
		uint32 quadOffset = 0;

		for (const StaticSurface& surface : surfaces) {
			const view<float32> vertexPositionComponents = surfaceGeometry->getVertexPositionComponents();
			const view<float32> vertexTextureCoordinateComponents = surfaceGeometry->getVertexTextureCoordinateComponents();
			const uint64 vertexCount = vertexPositionComponents.count() / 3;
			const uint64 textureCoordinateCount = vertexTextureCoordinateComponents.count() / 2;
			const view<uint32> vertexPositionIndices = surfaceGeometry->getVertexPositionIndices();
			const view<uint32> vertexNormalIndices = surfaceGeometry->getVertexNormalIndices();
			const view<uint32> vertexTextureCoordinateIndices = surfaceGeometry->getVertexTextureCoordinateIndices();
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
			const mat3x3<float32> textureWorld = (
				mat3x3<float32>::scaling(surface.transform.scale * vec2<float32>(surface.textureFiltering.scale)) *
				mat3x3<float32>::rotation(pi<float32>() * surface.textureFiltering.rotation) *
				mat3x3<float32>::translation(surface.textureFiltering.translation)
			);

			BuildStaticMaterial3D& material = scene.materials[surface.materialIndex];
			uint32 normalIndex;

			// Merge normal duplicates.
			{
				const svec3<float32> normal(normalized(vec3<float32>(face.normal) * inverse(orientation)));

				const svec3<float32> roundedNormal = {
					round(normal.x * 100.0f) / 100.0f,
					round(normal.y * 100.0f) / 100.0f,
					round(normal.z * 100.0f) / 100.0f,
				};

				view<svec3<float32>> normalView{};

				if (normals.size() > 0) {
					normalView = view<svec3<float32>>(&normals[0], normals.size());
				}

				normalIndex = findDuplicate(normalView, roundedNormal);

				if (normalIndex >= normals.size()) {
					normals.emplace_back(roundedNormal);
				}
			}

			sequence<uint32> textureCoordinateRedirectionIndices(textureCoordinateCount);

			// TODO: Merge texture coordinate duplicates.
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

			sequence<uint32> vertexRedirectionIndices(quadCount * 4);

			// Merge vertex duplicates.
			{
				const vec3<float32> origin = {vertexPositionComponents[0], vertexPositionComponents[1], vertexPositionComponents[2]};

				uint32 vertexOffset = 0;
				uint32 quadIndex = 0;

				while (quadIndex < surface.quads.size()) {
					const Quad& quad = surface.quads[quadIndex];
					const vec3<float32> quadScale = {quad.end.x - quad.start.x, 0.0f, quad.end.y - quad.start.y};

					pool<svec2<float32>, 4> quadVertices;
					quadVertices.add({quad.start.x, quad.start.y});
					quadVertices.add({quad.end.x, quad.start.y});
					quadVertices.add({quad.start.x, quad.end.y});
					quadVertices.add({quad.end.x, quad.end.y});

					uint32 vertexIndex = 0;

					while (vertexIndex < 4) {
						vec3<float32> vertex = origin;
						vertex += {quadVertices[vertexIndex].x / scale[0], 0.0f, -quadVertices[vertexIndex].y / scale[2]};
						vertex = multiplyTransform(vertex, world);

						const svec3<float32> roundedVertex = {
							round(vertex[0] * 100.0f) / 100.0f,
							round(vertex[1] * 100.0f) / 100.0f,
							round(vertex[2] * 100.0f) / 100.0f,
						};

						view<svec3<float32>> vertexView{};

						if (vertices.size() > 0) {
							vertexView = view<svec3<float32>>(&vertices[0], vertices.size());
						}

						const uint32 vertexRedirectionIndex = findDuplicate(vertexView, roundedVertex);

						if (vertexRedirectionIndex >= vertices.size()) {
							vertices.emplace_back(roundedVertex);
						}

						vertexRedirectionIndices[vertexOffset + vertexIndex] = vertexRedirectionIndex;

						vertexIndex += 1;
					}

					uint32 indexOffset = 0;

					while (indexOffset < vertexPositionIndices.count()) {
						scene.triangles[triangleIndex] = {
							.vertices = {
								{
									.vertexIndex = vertexRedirectionIndices[vertexOffset + vertexPositionIndices[indexOffset + 0]],
									// .textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 0]],
								},
								{
									.vertexIndex = vertexRedirectionIndices[vertexOffset + vertexPositionIndices[indexOffset + 1]],
									// .textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 1]],
								},
								{
									.vertexIndex = vertexRedirectionIndices[vertexOffset + vertexPositionIndices[indexOffset + 2]],
									// .textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexTextureCoordinateIndices[indexOffset + 2]],
								},
							},
							.normalIndex = normalIndex,
						};

						material.triangleRange.count += 1;
						indexOffset += 3;
						triangleIndex += 1;
					}

					vertexOffset += 4;
					quadIndex += 1;
				}
			}

			if (surface.materialIndex > 0) {
				const BuildStaticMaterial3D& previousMaterial = scene.materials[surface.materialIndex - 1];

				material.triangleRange.offset = previousMaterial.triangleRange.offset + previousMaterial.triangleRange.count;
			}

			const view<Edge3D> edges = surfaceGeometry->getHull3DEdges();
			const view<uint16> faceEdgeIndices = surfaceGeometry->getHull3DFaceEdgeIndices();

			uint32 quadIndex = 0;

			while (quadIndex < surface.quads.size()) {
				const Quad& quad = surface.quads[quadIndex];

				pool<svec2<float32>, 4> vertices;
				vertices.add({quad.start.x, quad.start.y});
				vertices.add({quad.end.x, quad.start.y});
				vertices.add({quad.start.x, quad.end.y});
				vertices.add({quad.end.x, quad.end.y});

				Lightmap& lightmap = lightmaps[quadOffset + quadIndex];
				lightmap.offset = {0, 0};
				lightmap.coordinateOffset = lightmapCoordinateOffset;

				svec2<float32> maxCoordinate = quad.end / surface.lightmapScale;
				maxCoordinate.x = round(maxCoordinate.x * 100.0f) / 100.0f;
				maxCoordinate.y = round(maxCoordinate.y * 100.0f) / 100.0f;

				DEBUG_ASSERT(maxCoordinate.x > 0.0f);

				svec2<float32> minCoordinate = quad.start / surface.lightmapScale;
				minCoordinate.x = round(minCoordinate.x * 100.0f) / 100.0f;
				minCoordinate.y = round(minCoordinate.y * 100.0f) / 100.0f;

				DEBUG_ASSERT(maxCoordinate.y > 0.0f);

				uint16 coordinateIndex = 0;

				while (coordinateIndex < 4) {
					svec2<float32>& coordinate = scene.lightmapCoordinates[lightmapCoordinateOffset + coordinateIndex];
					coordinate = vertices[coordinateIndex] / surface.lightmapScale;
					coordinate = {
						round(coordinate.x * 100.0f) / 100.0f,
						round(coordinate.y * 100.0f) / 100.0f,
					};

					coordinateIndex += 1;
				}

				// Pad for dilation.
				maxCoordinate.x += 2.0f;
				maxCoordinate.y += 2.0f;

				const svec2<float32> size = abs(maxCoordinate - minCoordinate);

				lightmap.size = {static_cast<uint32>(size.x), static_cast<uint32>(size.y)};

				LightmapIndex lightmapIndex{};
				lightmapIndex.index = quadOffset + quadIndex;
				lightmapIndex.area = lightmap.size.width * lightmap.size.height;

				uint32 lightmapInsertionIndex = 0;

				while (lightmapInsertionIndex < lightmapIndices.size()) {
					if (lightmapIndex.area > lightmapIndices[lightmapInsertionIndex].area) {
						break;
					}

					lightmapInsertionIndex += 1;
				}

				lightmapIndices.insert(lightmapIndices.begin() + lightmapInsertionIndex, lightmapIndex);

				lightmapCoordinateOffset += 4;
				quadIndex += 1;
			}

			quadOffset += quadIndex;
		}

		scene.vertices = sequence<svec3<float32>>(vertices.size());
		scene.normals = sequence<svec3<float32>>(normals.size());
		scene.textureCoordinates = sequence<svec2<float32>>(textureCoordinates.size());

		std::copy(vertices.begin(), vertices.end(), &scene.vertices[0]);
		std::copy(normals.begin(), normals.end(), &scene.normals[0]);
		std::copy(textureCoordinates.begin(), textureCoordinates.end(), &scene.textureCoordinates[0]);

		Lightmapper::updateAtlasCoordinates(scene, lightmaps, view<LightmapIndex>(&lightmapIndices[0], lightmapIndices.size()), surfaceGeometry);

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

	uint32 StaticSceneBuilder::carveSurfaces(sequence<StaticSurface>& surfaces) const {
		uint32 quadCount = 0;

		for (StaticSurface& surface : surfaces) {
			const svec2<float32> surfaceSize(surface.transform.scale);

			std::vector<Quad> queue = {
				{
					.start = {0.0f, 0.0f},
					.end = surfaceSize,
				},
			};

			while (queue.size() > 0) {
				Quad quad = queue.back();

				queue.pop_back();

				bool continueAfterBlockingCarving = false;
				svec2<float32> blockingCarvingEnd;
				float32 newOriginX = 0.0f;
				uint32 carvingIndex = 0;

				while (carvingIndex < surface.carvings.size()) {
					const SurfaceCarving& carving = surface.carvings[carvingIndex];

					// If the carving is not in the quad's horizontal bounds, continue.
					if (carving.offset.x + carving.scale.x <= quad.start.x || carving.offset.x > quad.end.x) {
						// If the carving was before the quad, at least adjust the quad's height.
						if (carving.offset.x + carving.scale.x == quad.start.x) {
							quad.end.y = min(quad.end.y, carving.offset.y + carving.scale.y);
						}

						carvingIndex += 1;

						continue;
					}

					// If the carving is not in the quad's vertical bounds, continue.
					if (carving.offset.y + carving.scale.y <= quad.start.y || carving.offset.y > quad.end.y) {
						carvingIndex += 1;

						continue;
					}

					// If the carving fully blocks the quad vertically, end the quad there.
					if (carving.offset.y <= quad.start.y && carving.offset.y + carving.scale.y >= quad.start.y) {
						quad.end.x = carving.offset.x;
						quad.end.y = min(quad.end.y, carving.offset.y + carving.scale.y);

						if (carving.offset.y == quad.start.y) {
							// There is no quad after the carving.
							continueAfterBlockingCarving = true;

							blockingCarvingEnd = carving.offset + carving.scale;
						}
						else {
							// There is already a quad after the carving.
							// Force a new quad to be added right below the current quad.
							newOriginX = quad.start.x;
						}

						carvingIndex += 1;

						continue;
					}

					// There is some space above the carving.
					quad.end.y = carving.offset.y;

					carvingIndex += 1;
				}

				surface.quads.emplace_back(quad);

				quadCount += 1;

				if (quad.start.x == newOriginX && quad.end != surfaceSize) {
					// If the quad started at the X origin, prepare a quad right below.
					queue.emplace_back(Quad{
						.start = {newOriginX, quad.end.y},
						.end = surfaceSize,
					});
				}

				if (continueAfterBlockingCarving) {
					// Prepare a quad after the carving.
					queue.emplace_back(Quad{
						.start = {blockingCarvingEnd.x, quad.start.y},
						.end = {surfaceSize.x, blockingCarvingEnd.y},
					});
				}
			}
		}

		return quadCount;
	}
}