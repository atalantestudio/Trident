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
	static uint32 findDuplicate(const std::vector<T>& elements, T a) {
		constexpr float32 E = 0.001f;

		uint32 index = 0;

		while (index < elements.size()) {
			const T b = elements[index];

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

		trianglesPerMaterial.resize(trianglesPerMaterial.size() + 1);
	}

	void StaticSceneBuilder::placeSurface(const SurfaceDescriptor& descriptor, const Transform& transform) {
		DEBUG_ASSERT(descriptor.geometry.scale.x > 0.0f && descriptor.geometry.scale.y > 0.0f);
		DEBUG_ASSERT(descriptor.luxelScale.x > 0.0f && descriptor.luxelScale.y > 0.0f);
		DEBUG_ASSERT(descriptor.carvings.size() == 0 || descriptor.geometry.vertexIndices.count() == 6);

		const quat<float32> orientation = quat<float32>::axisAngle(transform.axis, transform.angle);

		const mat4x4<float32> worldTransform = (
			mat4x4<float32>(orientation) *
			mat4x4<float32>::translation(transform.position)
		);
		const mat3x3<float32> textureTransform = (
			mat3x3<float32>::scaling(descriptor.geometry.scale * descriptor.textureFiltering.scale) *
			mat3x3<float32>::rotation(pi<float32>() * descriptor.textureFiltering.rotation) *
			mat3x3<float32>::translation(descriptor.textureFiltering.translation)
		);

		BuildStaticMaterial3D& material = materials[descriptor.materialIndex];
		std::vector<StaticTriangle3D>& triangles = trianglesPerMaterial[descriptor.materialIndex];

		StaticSurface surface{};
		surface.edges = descriptor.geometry.edges;

		// TODO(lyah/scalar)
		surface.normal = svec3<float32>(round<2>(normalized(descriptor.geometry.normal * inverse(orientation))));

		surface.scale = descriptor.geometry.scale;
		surface.subsurfaces = {};
		surface.textureFiltering = descriptor.textureFiltering;
		surface.triangleOffset = static_cast<uint32>(triangles.size());
		surface.materialIndex = descriptor.materialIndex;
		surface.luxelScale = descriptor.luxelScale;

		uint32 surfaceVertexCount = 0;

		if (descriptor.carvings.size() > 0) {
			// TODO(asset-builder/scene): Restore.
			// carveSurface(surface, descriptor.carvings, surfaceVertexCount);
		}
		else {
			surface.subsurfaces = {
				{
					.vertexComponents = descriptor.geometry.vertexComponents,
					.vertexTextureCoordinateComponents = descriptor.geometry.vertexTextureCoordinateComponents,
					.vertexIndices = descriptor.geometry.vertexIndices,
					.vertexTextureCoordinateIndices = descriptor.geometry.vertexTextureCoordinateIndices,
					.scale = surface.scale,
				},
			};

			surfaceVertexCount = static_cast<uint32>(surface.subsurfaces[0].vertexComponents.count() / 3);
		}

		const uint32 normalIndex = findDuplicate(normals, surface.normal);

		if (normalIndex >= normals.size()) {
			normals.emplace_back(surface.normal);
		}

		sequence<uint32> vertexRedirectionIndices(surfaceVertexCount);
		sequence<uint32> textureCoordinateRedirectionIndices(surfaceVertexCount);
		uint32 subsurfaceIndex = 0;
		uint32 textureCoordinateOffset = 0;
		uint32 vertexIndexOffset = 0;

		while (subsurfaceIndex < surface.subsurfaces.size()) {
			const StaticSubsurface& subsurface = surface.subsurfaces[subsurfaceIndex];
			const uint32 vertexIndexCount = static_cast<uint32>(subsurface.vertexIndices.count());
			const uint64 vertexCount = subsurface.vertexComponents.count() / 3;
			const uint64 textureCoordinateCount = subsurface.vertexTextureCoordinateComponents.count() / 2;

			uint32 textureCoordinateIndex = 0;

			while (textureCoordinateIndex < textureCoordinateCount) {
				// TODO(lyah/scalar)
				const svec2<float32> textureCoordinate = svec2<float32>(multiplyTransform(vec2<float32>{
					subsurface.vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 0],
					subsurface.vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 1],
				} / vec2<float32>(subsurface.scale), textureTransform));

				const uint32 textureCoordinateRedirectionIndex = findDuplicate(textureCoordinates, textureCoordinate);

				if (textureCoordinateRedirectionIndex >= textureCoordinates.size()) {
					textureCoordinates.emplace_back(textureCoordinate);
				}

				textureCoordinateRedirectionIndices[textureCoordinateOffset + textureCoordinateIndex] = textureCoordinateRedirectionIndex;

				textureCoordinateIndex += 1;
			}

			textureCoordinateOffset += textureCoordinateIndex;

			uint32 vertexIndex = 0;

			while (vertexIndex < vertexCount) {
				// TODO(lyah/scalar)
				const svec3<float32> vertex = svec3<float32>(round<2>(multiplyTransform({
					subsurface.vertexComponents[vertexIndex * 3 + 0],
					subsurface.vertexComponents[vertexIndex * 3 + 1],
					subsurface.vertexComponents[vertexIndex * 3 + 2],
				}, worldTransform)));

				const uint32 vertexRedirectionIndex = findDuplicate(vertices, vertex);

				if (vertexRedirectionIndex >= vertices.size()) {
					vertices.emplace_back(vertex);
				}

				vertexRedirectionIndices[vertexIndexOffset + vertexIndex] = vertexRedirectionIndex;

				vertexIndex += 1;
			}

			uint32 vertexIndexIndex = 0;

			while (vertexIndexIndex < vertexIndexCount) {
				const uint32 vertexIndexA = vertexIndexOffset + subsurface.vertexIndices[vertexIndexIndex + 0];
				const uint32 vertexIndexB = vertexIndexOffset + subsurface.vertexIndices[vertexIndexIndex + 1];
				const uint32 vertexIndexC = vertexIndexOffset + subsurface.vertexIndices[vertexIndexIndex + 2];

				const uint32 textureCoordinateIndexA = vertexIndexOffset + subsurface.vertexTextureCoordinateIndices[vertexIndexIndex + 0];
				const uint32 textureCoordinateIndexB = vertexIndexOffset + subsurface.vertexTextureCoordinateIndices[vertexIndexIndex + 1];
				const uint32 textureCoordinateIndexC = vertexIndexOffset + subsurface.vertexTextureCoordinateIndices[vertexIndexIndex + 2];

				const StaticTriangle3D triangle = {
					.vertices = {
						{
							.vertexIndex = vertexRedirectionIndices[vertexIndexA],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[textureCoordinateIndexA],
							.lightmapCoordinateIndex = 0,
						},
						{
							.vertexIndex = vertexRedirectionIndices[vertexIndexB],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[textureCoordinateIndexB],
							.lightmapCoordinateIndex = 0,
						},
						{
							.vertexIndex = vertexRedirectionIndices[vertexIndexC],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[textureCoordinateIndexC],
							.lightmapCoordinateIndex = 0,
						},
					},
					.normalIndex = normalIndex,
				};

				triangles.emplace_back(triangle);

				vertexIndexIndex += 3;
				subsurfaceIndex += 1;
			}

			material.triangleRange.count += vertexIndexCount / 3;
			triangleCount += vertexIndexCount / 3;
			vertexIndexOffset += vertexIndexCount;
			subsurfaceIndex += 1;
		}

		surfaces.emplace_back(surface);

		subsurfaceCount += static_cast<uint32>(surface.subsurfaces.size());
	}

	StaticScene3D StaticSceneBuilder::build() const {
		sequence<StaticSurface> surfaces(this->surfaces.size());

		std::copy(this->surfaces.begin(), this->surfaces.end(), &surfaces[0]);

		StaticScene3D scene{};
		scene.lightmapCoordinates = sequence<svec2<float32>>(subsurfaceCount * 4);
		scene.triangles = sequence<StaticTriangle3D>(triangleCount);
		scene.materialShaders = sequence<BuildMaterialShader>(materialShaders.size());
		scene.materials = sequence<BuildStaticMaterial3D>(materials.size());

		// TODO(asset-builder/scene): Restore.
		/* sequence<std::vector<StaticTriangle3D>> trianglesPerMaterial(scene.materials.count());
		sequence<Lightmap> lightmaps(subsurfaceCount);

		// Sorted by ascending lightmap area.
		std::vector<LightmapIndex> lightmapIndices;
		lightmapIndices.reserve(subsurfaceCount); */

		std::copy(materialShaders.begin(), materialShaders.end(), &scene.materialShaders[0]);
		std::copy(materials.begin(), materials.end(), &scene.materials[0]);

		uint32 materialIndex = 0;
		uint32 triangleOffset = 0;

		while (materialIndex < scene.materials.count()) {
			const std::vector<StaticTriangle3D>& triangles = trianglesPerMaterial[materialIndex];

			if (triangles.size() == 0) {
				materialIndex += 1;

				continue;
			}

			std::copy(triangles.begin(), triangles.end(), &scene.triangles[triangleOffset]);

			scene.materials[materialIndex].triangleRange.offset = triangleOffset;

			triangleOffset += static_cast<uint32>(triangles.size());
			materialIndex += 1;
		}

		// TODO(asset-builder/scene): Restore.
		/* uint32 quadOffset = 0;

		for (const StaticSurface& surface : surfaces) {
			const uint32 triangleOffset = scene.materials[surface.materialIndex].triangleRange.offset + surface.triangleOffset;

			uint32 quadIndex = 0;

			while (quadIndex < surface.subsurfaces.size()) {
				const StaticSubsurface& quad = surface.subsurfaces[quadIndex];

				pool<svec2<float32>, 4> vertices;
				vertices.add({0.0f, 0.0f});
				vertices.add({quad.end.x - quad.start.x, 0.0f});
				vertices.add({0.0f, quad.end.y - quad.start.y});
				vertices.add({quad.end - quad.start});

				Lightmap& lightmap = lightmaps[quadOffset + quadIndex];
				lightmap.triangleOffset = triangleOffset + quadIndex * 2;
				lightmap.offset = {0, 0};
				lightmap.coordinateOffset = triangleOffset * 2 + quadIndex * 4;

				svec2<float32> maxCoordinate = (quad.end - quad.start) / surface.luxelScale;
				maxCoordinate.x = round(maxCoordinate.x * 100.0f) / 100.0f;
				maxCoordinate.y = round(maxCoordinate.y * 100.0f) / 100.0f;

				DEBUG_ASSERT(maxCoordinate.x > 0.0f);

				uint16 coordinateIndex = 0;

				while (coordinateIndex < vertices.count()) {
					svec2<float32>& coordinate = scene.lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex];
					coordinate = vertices[coordinateIndex] / surface.luxelScale;
					coordinate.x = round(coordinate.x * 100.0f) / 100.0f;
					coordinate.y = round(coordinate.y * 100.0f) / 100.0f;

					coordinateIndex += 1;
				}

				// Pad for dilation.
				maxCoordinate.x += 2.0f;
				maxCoordinate.y += 2.0f;

				const svec2<float32> size = abs(maxCoordinate);

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

				quadIndex += 1;
			}

			quadOffset += quadIndex;
		} */

		scene.vertices = sequence<svec3<float32>>(vertices.size());
		scene.normals = sequence<svec3<float32>>(normals.size());
		scene.textureCoordinates = sequence<svec2<float32>>(textureCoordinates.size());

		std::copy(vertices.begin(), vertices.end(), &scene.vertices[0]);
		std::copy(normals.begin(), normals.end(), &scene.normals[0]);
		std::copy(textureCoordinates.begin(), textureCoordinates.end(), &scene.textureCoordinates[0]);

		// TODO(asset-builder/scene): Restore.
		/* Lightmapper::updateAtlasCoordinates(scene, lightmaps, view<LightmapIndex>(&lightmapIndices[0], lightmapIndices.size()));

		scene.lightmapAtlasImageAssetDescriptor = {
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {Lightmapper::ATLAS_SIZE.width, Lightmapper::ATLAS_SIZE.width, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.data = sequence<uint8>(Lightmapper::ATLAS_SIZE.width * Lightmapper::ATLAS_SIZE.height * getByteSize(ImageFormat::R8G8B8A8_UNORM)),
		};
		scene.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();

		Lightmapper lightmapper;
		lightmapper.buildLightmapAtlas(scene, scene.lightmapAtlasImageAssetDescriptor); */

		return scene;
	}

	// TODO(asset-builder/scene): Restore.
	/* void StaticSceneBuilder::carveSurface(StaticSurface& surface, const std::vector<Carving>& carvings) const {
		std::vector<SubsurfaceQuad> resultQuads;
		std::vector<SubsurfaceQuad> queue = {
			{
				.start = {0.0f, 0.0f},
				.end = surface.scale,
			},
		};

		while (queue.size() > 0) {
			SubsurfaceQuad quad = queue.back();

			queue.pop_back();

			bool continueAfterBlockingCarving = false;
			svec2<float32> blockingCarvingEnd;
			float32 newOriginX = 0.0f;
			uint32 carvingIndex = 0;

			while (carvingIndex < carvings.size()) {
				const Carving& carving = carvings[carvingIndex];

				// If the carving is not in the quad's horizontal bounds, continue.
				if (carving.offset.x + carving.size.x <= quad.start.x || carving.offset.x > quad.end.x) {
					// If the carving was before the quad, at least adjust the quad's height.
					if (carving.offset.x + carving.size.x == quad.start.x) {
						quad.end.y = min(quad.end.y, carving.offset.y + carving.size.y);
					}

					carvingIndex += 1;

					continue;
				}

				// If the carving is not in the quad's vertical bounds, continue.
				if (carving.offset.y + carving.size.y <= quad.start.y || carving.offset.y > quad.end.y) {
					carvingIndex += 1;

					continue;
				}

				// If the carving fully blocks the quad vertically, end the quad there.
				if (carving.offset.y <= quad.start.y && carving.offset.y + carving.size.y >= quad.start.y) {
					quad.end.x = carving.offset.x;
					quad.end.y = min(quad.end.y, carving.offset.y + carving.size.y);

					if (carving.offset.y == quad.start.y) {
						// There is no quad after the carving.
						continueAfterBlockingCarving = true;

						blockingCarvingEnd = carving.offset + carving.size;
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

			surface.subsurfaces.emplace_back(subsurface);

			if (quad.start.x == newOriginX && quad.end != surface.scale) {
				// If the quad started at the X origin, prepare a quad right below.
				queue.emplace_back(SubsurfaceQuad{
					.start = {newOriginX, quad.end.y},
					.end = surface.scale,
				});
			}

			if (continueAfterBlockingCarving) {
				// Prepare a quad after the carving.
				queue.emplace_back(SubsurfaceQuad{
					.start = {blockingCarvingEnd.x, quad.start.y},
					.end = {surface.scale.x, blockingCarvingEnd.y},
				});
			}
		}
	} */
}