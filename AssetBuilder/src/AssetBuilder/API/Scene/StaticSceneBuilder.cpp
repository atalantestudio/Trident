// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/Carving.hpp"
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

	StaticGeometry3D StaticSceneBuilder::createWallGeometry(svec2<float32> scale) {
		return {
			.vertexComponents = {
				-0.5f * scale.x,  0.5f * scale.y,  0.0f,
				 0.5f * scale.x,  0.5f * scale.y,  0.0f,
				-0.5f * scale.x, -0.5f * scale.y,  0.0f,
				 0.5f * scale.x, -0.5f * scale.y,  0.0f,
			},
			.vertexTextureCoordinateComponents = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f,
			},
			.vertexIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.vertexTextureCoordinateIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.edgeOriginVertexIndices = {0, 1, 3, 2},
			.normal = {0.0f, 0.0f, -1.0f},
			.scale = scale,
		};
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

	uint32 StaticSceneBuilder::placeSurface(const SurfaceDescriptor& descriptor, const Transform& transform) {
		// TODO(asset-builder/scene): Better validation.
		DEBUG_ASSERT(descriptor.geometry.scale.x > 0.0f && descriptor.geometry.scale.y > 0.0f);
		DEBUG_ASSERT(descriptor.luxelScale.x > 0.0f && descriptor.luxelScale.y > 0.0f);
		DEBUG_ASSERT(descriptor.carvings.size() == 0 || descriptor.geometry.vertexIndices.count() == 6);

		return compileSurface(descriptor, transform, {0.0f, 0.0f, 0.0f}, quat<float32>::identity());
	}

	uint32 StaticSceneBuilder::snapSurface(const SurfaceDescriptor& descriptor, const StaticEdge& parentEdge, uint32 edgeIndex, float32 hingeAngle) {
		// TODO(asset-builder/scene): Better validation.
		DEBUG_ASSERT(descriptor.geometry.scale.x > 0.0f && descriptor.geometry.scale.y > 0.0f);
		DEBUG_ASSERT(descriptor.luxelScale.x > 0.0f && descriptor.luxelScale.y > 0.0f);
		DEBUG_ASSERT(descriptor.carvings.size() == 0 || descriptor.geometry.vertexIndices.count() == 6);

		const uint16 indexC = descriptor.geometry.edgeOriginVertexIndices[edgeIndex];
		const uint16 indexD = descriptor.geometry.edgeOriginVertexIndices[(edgeIndex + 1) % descriptor.geometry.edgeOriginVertexIndices.count()];

		const vec3<float32> a = parentEdge.a;
		const vec3<float32> b = parentEdge.b;
		const vec3<float32> localC = {
			descriptor.geometry.vertexComponents[indexC * 3 + 0],
			descriptor.geometry.vertexComponents[indexC * 3 + 1],
			descriptor.geometry.vertexComponents[indexC * 3 + 2],
		};
		const vec3<float32> localD = {
			descriptor.geometry.vertexComponents[indexD * 3 + 0],
			descriptor.geometry.vertexComponents[indexD * 3 + 1],
			descriptor.geometry.vertexComponents[indexD * 3 + 2],
		};

		const vec3<float32> ab = normalized(b - a);
		const vec3<float32> localCD = normalized(localD - localC);

		DEBUG_ASSERT(lengthSquared(ab) == lengthSquared(localCD));

		vec3<float32> snapAxis = ab;
		float32 snapAngle = 0.0_rad;

		if (!parallelOrOpposite(ab, localCD)) {
			snapAxis = normalized(cross(localCD, ab));
			snapAngle = std::acos(dot(snapAxis, descriptor.geometry.normal));
		}

		const quat<float32> snapOrientation = quat<float32>::axisAngle(snapAxis, snapAngle);
		const vec3<float32> c = localC * inverse(snapOrientation);
		const vec3<float32> cb = b - c;

		const Transform transform = {
			.translation = cb,
			.axis = svec3<float32>(snapAxis),
			.angle = snapAngle,
		};

		const quat<float32> hingeRotation = quat<float32>::axisAngle(ab, hingeAngle);

		return compileSurface(descriptor, transform, c, hingeRotation);
	}

	uint32 StaticSceneBuilder::snapSurface(const DualSnapSurfaceDescriptor& descriptor, const StaticEdge& edgeA, const StaticEdge& edgeB) {
		// TODO(asset-builder/scene): Better validation.
		DEBUG_ASSERT(descriptor.luxelScale.x > 0.0f && descriptor.luxelScale.y > 0.0f);

		const vec3<float32> a = edgeA.a;
		const vec3<float32> b = edgeA.b;
		const vec3<float32> c = edgeB.a;
		const vec3<float32> d = edgeB.b;

		DEBUG_ASSERT(a != c && a != d && b != c && b != d);

		const vec3<float32> ab = normalized(b - a);
		const vec3<float32> cd = normalized(d - c);

		DEBUG_ASSERT(dot(ab, cd) <= 0.0f);

		StaticGeometry3D geometry{};
		geometry.vertexComponents = {
			c[0], c[1], c[2],
			d[0], d[1], d[2],
			b[0], b[1], b[2],
			a[0], a[1], a[2],
		};
		geometry.vertexTextureCoordinateComponents = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
		};
		geometry.vertexIndices = {
			0, 1, 2,
			1, 3, 2,
		};
		geometry.vertexTextureCoordinateIndices = {
			0, 1, 2,
			1, 3, 2,
		};
		geometry.edgeOriginVertexIndices = {0, 1, 3, 2};
		geometry.normal = normalized(cross(ab, c - b));
		geometry.scale = {distance(a, b), distance(c, b)};

		const SurfaceDescriptor surfaceDescriptor = {
			.geometry = geometry,
			.textureFiltering = descriptor.textureFiltering,
			.materialIndex = descriptor.materialIndex,
			.luxelScale = descriptor.luxelScale,
			.carvings = descriptor.carvings,
		};

		// TODO(asset-builder/scene/dual-snapping): compileSurface overload without transform.
		return compileSurface(surfaceDescriptor, {}, {0.0f, 0.0f, 0.0f}, quat<float32>::identity());
	}

	uint32 StaticSceneBuilder::compileSurface(const SurfaceDescriptor& descriptor, const Transform& transform, vec3<float32> hingeOffset, quat<float32> hingeRotation) {
		const uint32 surfaceIndex = static_cast<uint32>(surfaces.size());
		const quat<float32> orientation = quat<float32>::axisAngle(transform.axis, transform.angle);
		const mat4x4<float32> worldTransform = (
			mat4x4<float32>(orientation) *
			mat4x4<float32>::translation(-hingeOffset) *
			mat4x4<float32>(hingeRotation) *
			mat4x4<float32>::translation(hingeOffset + transform.translation)
		);
		const mat3x3<float32> textureTransform = (
			mat3x3<float32>::scaling(descriptor.geometry.scale * descriptor.textureFiltering.scale) *
			mat3x3<float32>::rotation(pi<float32>() * descriptor.textureFiltering.rotation) *
			mat3x3<float32>::translation(descriptor.textureFiltering.translation)
		);

		BuildStaticMaterial3D& material = materials[descriptor.materialIndex];
		std::vector<StaticTriangle3D>& triangles = trianglesPerMaterial[descriptor.materialIndex];

		StaticSurface surface{};
		surface.vertices = sequence<svec3<float32>>(descriptor.geometry.vertexComponents.count() / 3);
		surface.edgeOriginVertexIndices = descriptor.geometry.edgeOriginVertexIndices;

		// TODO(lyah/scalar)
		surface.normal = svec3<float32>(round<2>(normalized(descriptor.geometry.normal * inverse(orientation))));

		surface.scale = descriptor.geometry.scale;
		surface.subsurfaces = {};
		surface.textureFiltering = descriptor.textureFiltering;
		surface.triangleOffset = static_cast<uint32>(triangles.size());
		surface.materialIndex = descriptor.materialIndex;
		surface.luxelScale = descriptor.luxelScale;

		uint32 vertexIndex = 0;

		while (vertexIndex < surface.vertices.count()) {
			// TODO(lyah/scalar)
			surface.vertices[vertexIndex] = svec3<float32>(round<2>(multiplyTransform({
				descriptor.geometry.vertexComponents[vertexIndex * 3 + 0],
				descriptor.geometry.vertexComponents[vertexIndex * 3 + 1],
				descriptor.geometry.vertexComponents[vertexIndex * 3 + 2],
			}, worldTransform)));

			vertexIndex += 1;
		}

		uint32 surfaceVertexCount = 0;

		if (descriptor.carvings.size() > 0) {
			for (const Carving& carving : descriptor.carvings) {
				DEBUG_ASSERT(carving.size.x > 0.0f && carving.size.y > 0.0f);
			}

			const svec3<float32> origin = {descriptor.geometry.vertexComponents[0], descriptor.geometry.vertexComponents[1], descriptor.geometry.vertexComponents[2]};

			carveSurface(surface, origin, descriptor.carvings, surfaceVertexCount);

			uint32 hollowSubsurfaceIndex = 0;

			while (hollowSubsurfaceIndex < surface.hollowSubsurfaces.size()) {
				StaticSubsurface& subsurface = surface.hollowSubsurfaces[hollowSubsurfaceIndex];

				const uint32 vertexCount = static_cast<uint32>(subsurface.vertices.count());

				uint32 vertexIndex = 0;

				while (vertexIndex < vertexCount) {
					// TODO(lyah/scalar)
					// Subsurfaces must have world space vertices for snapping.
					subsurface.vertices[vertexIndex] = svec3<float32>(round<2>(multiplyTransform(vec3<float32>(subsurface.vertices[vertexIndex]), worldTransform)));

					vertexIndex += 1;
				}

				hollowSubsurfaceIndex += 1;
			}
		}
		else {
			surface.subsurfaces = {
				{
					.vertices = sequence<svec3<float32>>(surface.vertices.count()),
					.vertexTextureCoordinateComponents = descriptor.geometry.vertexTextureCoordinateComponents,
					.vertexIndices = descriptor.geometry.vertexIndices,
					.vertexTextureCoordinateIndices = descriptor.geometry.vertexTextureCoordinateIndices,
					.scale = surface.scale,
				},
			};

			uint32 vertexIndex = 0;

			while (vertexIndex < surface.vertices.count()) {
				surface.subsurfaces[0].vertices[vertexIndex] = {
					descriptor.geometry.vertexComponents[vertexIndex * 3 + 0],
					descriptor.geometry.vertexComponents[vertexIndex * 3 + 1],
					descriptor.geometry.vertexComponents[vertexIndex * 3 + 2],
				};

				vertexIndex += 1;
			}

			surfaceVertexCount = static_cast<uint32>(surface.vertices.count());
		}

		const uint32 normalIndex = findDuplicate(normals, surface.normal);

		if (normalIndex >= normals.size()) {
			normals.emplace_back(surface.normal);
		}

		// Worst-case allocation.
		sequence<uint32> vertexRedirectionIndices(surfaceVertexCount);
		sequence<uint32> textureCoordinateRedirectionIndices(surfaceVertexCount);

		uint32 surfaceTriangleCount = 0;
		uint32 subsurfaceIndex = 0;
		uint32 textureCoordinateOffset = 0;
		uint32 vertexOffset = 0;

		while (subsurfaceIndex < surface.subsurfaces.size()) {
			StaticSubsurface& subsurface = surface.subsurfaces[subsurfaceIndex];

			const uint64 vertexIndexCount = subsurface.vertexIndices.count();
			const uint64 vertexCount = subsurface.vertices.count();
			const uint64 textureCoordinateCount = subsurface.vertexTextureCoordinateComponents.count() / 2;

			uint32 textureCoordinateIndex = 0;

			while (textureCoordinateIndex < textureCoordinateCount) {
				// TODO(lyah/scalar)
				svec2<float32> textureCoordinate = svec2<float32>(multiplyTransform({
					subsurface.vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 0],
					subsurface.vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 1],
				}, textureTransform));

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
				const svec3<float32> vertex = svec3<float32>(round<2>(multiplyTransform(vec3<float32>(subsurface.vertices[vertexIndex]), worldTransform)));

				// Subsurfaces must have world space vertices for snapping.
				subsurface.vertices[vertexIndex] = vertex;

				const uint32 vertexRedirectionIndex = findDuplicate(vertices, vertex);

				if (vertexRedirectionIndex >= vertices.size()) {
					vertices.emplace_back(vertex);
				}

				vertexRedirectionIndices[vertexOffset + vertexIndex] = vertexRedirectionIndex;

				vertexIndex += 1;
			}

			uint32 vertexIndexIndex = 0;

			while (vertexIndexIndex < vertexIndexCount) {
				const uint32 vertexIndexA = subsurface.vertexIndices[vertexIndexIndex + 0];
				const uint32 vertexIndexB = subsurface.vertexIndices[vertexIndexIndex + 1];
				const uint32 vertexIndexC = subsurface.vertexIndices[vertexIndexIndex + 2];

				const uint32 textureCoordinateIndexA = subsurface.vertexTextureCoordinateIndices[vertexIndexIndex + 0];
				const uint32 textureCoordinateIndexB = subsurface.vertexTextureCoordinateIndices[vertexIndexIndex + 1];
				const uint32 textureCoordinateIndexC = subsurface.vertexTextureCoordinateIndices[vertexIndexIndex + 2];

				const StaticTriangle3D triangle = {
					.vertices = {
						{
							.vertexIndex = vertexRedirectionIndices[vertexOffset + vertexIndexA],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexOffset + textureCoordinateIndexA],
							.lightmapCoordinateIndex = 0,
						},
						{
							.vertexIndex = vertexRedirectionIndices[vertexOffset + vertexIndexB],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexOffset + textureCoordinateIndexB],
							.lightmapCoordinateIndex = 0,
						},
						{
							.vertexIndex = vertexRedirectionIndices[vertexOffset + vertexIndexC],
							.textureCoordinateIndex = textureCoordinateRedirectionIndices[vertexOffset + textureCoordinateIndexC],
							.lightmapCoordinateIndex = 0,
						},
					},
					.normalIndex = normalIndex,
				};

				triangles.emplace_back(triangle);

				vertexIndexIndex += 3;
			}

			surfaceTriangleCount += static_cast<uint32>(vertexIndexCount / 3);
			vertexOffset += static_cast<uint32>(vertexCount);
			subsurfaceIndex += 1;
		}

		surfaces.emplace_back(surface);

		material.triangleRange.count += surfaceTriangleCount;
		triangleCount += surfaceTriangleCount;
		subsurfaceCount += static_cast<uint32>(surface.subsurfaces.size());

		return surfaceIndex;
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
}