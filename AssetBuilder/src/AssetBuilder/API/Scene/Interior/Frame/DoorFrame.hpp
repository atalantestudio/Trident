// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Frame/Frame.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/RectangularRoom.hpp"

namespace Trident {
	struct DoorFrameDescriptor {
		/// Entrance room properties.
		RectangularRoom* roomA;
		uint32 surfaceIndexA;
		float32 horizontalOffsetA;

		/// Exit room properties.
		RectangularRoom* roomB;
		uint32 surfaceIndexB;
		float32 horizontalOffsetB;

		/// Door and frame properties.
		svec2<float32> doorSize;
		float32 frameThickness;
		uint32 frameMaterialIndex;
		svec2<float32> frameLuxelScale;
	};

	struct DoorFrame : public Frame {
		static constexpr uint32 RIGHT_JAMB_A = 0;
		static constexpr uint32 LEFT_JAMB_A = 1;
		static constexpr uint32 TOP_JAMB_A = 2;
		static constexpr uint32 RIGHT_CASING_A = 3;
		static constexpr uint32 LEFT_CASING_A = 4;
		static constexpr uint32 TOP_CASING_A = 5;
		static constexpr uint32 RIGHT_CASING_B = 6;
		static constexpr uint32 LEFT_CASING_B = 7;
		static constexpr uint32 TOP_CASING_B = 8;

		explicit DoorFrame(const DoorFrameDescriptor& descriptor) :
			doorSize(descriptor.doorSize),
			frameThickness(descriptor.frameThickness),
			frameMaterialIndex(descriptor.frameMaterialIndex),
			frameLuxelScale(descriptor.frameLuxelScale)
		{
			DEBUG_ASSERT(descriptor.roomA != nullptr);
			DEBUG_ASSERT(descriptor.roomB != nullptr);

			const SurfaceDescriptor& floorDescriptorA = descriptor.roomA->surfaceDescriptors[RectangularRoom::FLOOR];

			floorDescriptor = {
				.textureFiltering = {},
				.materialIndex = floorDescriptorA.materialIndex,
				.lightmap = {
					.luxelScale = floorDescriptorA.lightmap.luxelScale,
					.tintColor = {1.0f, 1.0f, 1.0f},
				},
			};

			// TODO
			const TextureFilteringDescriptor textureFiltering = {
				.translation = {0.195f, 0.1575f},
				.rotation = 0.0f,
				.scale = {-0.25f * 256.0f, 0.25f * 512.0f},
			};

			frameTextureFilterings = {
				// Right jamb A
				{
					.translation = {0.0625f, 0.1575f},
					.rotation = 0.0f,
					.scale = {0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Left jamb A
				{
					.translation = {0.195f, 0.1575f},
					.rotation = 0.0f,
					.scale = {-0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Top jamb A
				{
					.translation = {0.0625f, 0.6075f},
					.rotation = 0.0f,
					.scale = {0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Right casing A
				{
					.translation = {0.0625f, 0.155f},
					.rotation = 0.0f,
					.scale = {-0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Left casing A
				{
					.translation = {0.875f, 0.155f},
					.rotation = 0.0f,
					.scale = {-0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Top casing A
				{
					.translation = {-0.125f, 0.125f},
					.rotation = 0.0f,
					.scale = {-0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Right casing B
				{
					.translation = {0.0f, 0.155f},
					.rotation = 0.0f,
					.scale = {0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Left casing B
				{
					.translation = {0.8125f, 0.155f},
					.rotation = 0.0f,
					.scale = {0.25f * 256.0f, 0.25f * 512.0f},
				},

				// Top casing B
				{
					.translation = {0.0f, 0.125f},
					.rotation = 0.0f,
					.scale = {0.25f * 256.0f, 0.25f * 512.0f},
				},
			};

			direction = descriptor.roomB->position - descriptor.roomA->position;

			floorIndexA = &descriptor.roomA->surfaceIndices[RectangularRoom::FLOOR];
			wallIndexA = &descriptor.roomA->surfaceIndices[descriptor.surfaceIndexA];
			floorIndexB = &descriptor.roomB->surfaceIndices[RectangularRoom::FLOOR];
			wallIndexB = &descriptor.roomB->surfaceIndices[descriptor.surfaceIndexB];

			SurfaceDescriptor& surfaceDescriptorA = descriptor.roomA->surfaceDescriptors[descriptor.surfaceIndexA];
			SurfaceDescriptor& surfaceDescriptorB = descriptor.roomB->surfaceDescriptors[descriptor.surfaceIndexB];

			subsurfaceIndexA = static_cast<uint32>(surfaceDescriptorA.carvings.size());
			subsurfaceIndexB = static_cast<uint32>(surfaceDescriptorB.carvings.size());

			// TODO(lyah/scalar)
			const svec2<float32> frameSize = descriptor.doorSize + svec2<float32>{descriptor.frameThickness * 2.0f, descriptor.frameThickness};

			surfaceDescriptorA.carvings.emplace_back(Carving{
				.offset = {descriptor.horizontalOffsetA, surfaceDescriptorA.geometry.scale.y - frameSize.y},
				.size = frameSize,
			});

			surfaceDescriptorB.carvings.emplace_back(Carving{
				.offset = {descriptor.horizontalOffsetB, surfaceDescriptorB.geometry.scale.y - frameSize.y},
				.size = frameSize,
			});
		}

		SurfaceDescriptor getFrameDescriptor(uint32 index, StaticGeometry3D geometry) const {
			return {
				.geometry = geometry,
				.textureFiltering = frameTextureFilterings[index],
				.materialIndex = frameMaterialIndex,
				.lightmap = {
					.luxelScale = frameLuxelScale,
					.tintColor = {1.0f, 1.0f, 1.0f},
				},
			};
		}

		DualSnapSurfaceDescriptor floorDescriptor;
		sequence<TextureFilteringDescriptor> frameTextureFilterings;
		vec3<float32> direction;
		const uint32* floorIndexA;
		const uint32* wallIndexA;
		uint32 subsurfaceIndexA;
		const uint32* floorIndexB;
		const uint32* wallIndexB;
		uint32 subsurfaceIndexB;
		svec2<float32> doorSize;
		float32 frameThickness;
		uint32 frameMaterialIndex;
		svec2<float32> frameLuxelScale;
	}; 

	template<>
	inline void InteriorSceneBuilder::addFrame(DoorFrame& frame) {
		const StaticSubsurface& subsurfaceA = surfaces[*frame.wallIndexA].hollowSubsurfaces[frame.subsurfaceIndexA];
		const StaticSubsurface& subsurfaceB = surfaces[*frame.wallIndexB].hollowSubsurfaces[frame.subsurfaceIndexB];

		uint32 floorIndex;
		float32 frameLength;
		vec3<float32> ab;
		StaticEdge edge;

		// Snap floor.
		{
			float32 l;

			DualSnapEdge edgeA = surfaces[*frame.floorIndexA].getDualSnapEdge(frame.direction);
			StaticEdge subsurfaceEdgeA = subsurfaceA.getEdge(2);

			ab = normalized(subsurfaceEdgeA.b - subsurfaceEdgeA.a);

			subsurfaceEdgeA.a += ab * frame.frameThickness;
			subsurfaceEdgeA.b -= ab * frame.frameThickness;

			l = length(edgeA.vertexB - edgeA.vertexA);

			const float32 lA = length(subsurfaceEdgeA.a - edgeA.vertexB);
			const float32 lB = length(subsurfaceEdgeA.b - edgeA.vertexA);
			const vec2<float32> uvA = lerp(edgeA.uvB, edgeA.uvA, lA / l);
			const vec2<float32> uvB = lerp(edgeA.uvA, edgeA.uvB, lB / l);

			edgeA.vertexA = subsurfaceEdgeA.a;
			edgeA.vertexB = subsurfaceEdgeA.b;
			edgeA.uvA = uvA;
			edgeA.uvB = uvB;

			DualSnapEdge edgeB = surfaces[*frame.floorIndexB].getDualSnapEdge(-frame.direction);
			StaticEdge subsurfaceEdgeB = subsurfaceB.getEdge(2);

			const vec3<float32> cd = normalized(subsurfaceEdgeB.b - subsurfaceEdgeB.a);

			subsurfaceEdgeB.a += cd * frame.frameThickness;
			subsurfaceEdgeB.b -= cd * frame.frameThickness;

			l = length(edgeB.vertexB - edgeB.vertexA);

			const float32 lC = length(subsurfaceEdgeB.a - edgeB.vertexB);
			const float32 lD = length(subsurfaceEdgeB.b - edgeB.vertexA);
			const vec2<float32> uvC = lerp(edgeB.uvB, edgeB.uvA, lC / l);
			const vec2<float32> uvD = lerp(edgeB.uvA, edgeB.uvB, lD / l);

			edgeB.vertexA = subsurfaceEdgeB.a;
			edgeB.vertexB = subsurfaceEdgeB.b;
			edgeB.uvA = uvC;
			edgeB.uvB = uvD;

			frameLength = length(edgeA.vertexB - edgeB.vertexA);

			const vec2<float32> cb = normalized(edgeA.uvB - edgeB.uvA);

			edgeB.uvA += cb;
			edgeB.uvB += cb;

			floorIndex = snapSurface(frame.floorDescriptor, edgeA, edgeB);
		}

		const StaticGeometry3D verticalJambGeometry = createWallGeometry({frameLength, frame.doorSize.y});
		const StaticGeometry3D horizontalJambGeometry = createWallGeometry({frameLength, frame.doorSize.x});
		const StaticGeometry3D verticalCasingGeometry = createWallGeometry({frame.frameThickness, frame.doorSize.y});
		const StaticGeometry3D horizontalCasingGeometry = createWallGeometry({frame.doorSize.x + frame.frameThickness * 2.0f, frame.frameThickness});

		// Right jamb A
		const uint32 rightJambIndexA = snapSurface(frame.getFrameDescriptor(frame.RIGHT_JAMB_A, verticalJambGeometry), surfaces[floorIndex].getEdge(0), 2, 0.0_rad);

		edge = surfaces[rightJambIndexA].getEdge(1);
		edge.a += ab * frame.doorSize.x;
		edge.b += ab * frame.doorSize.x;

		const uint32 leftJambIndexA = snapSurface(frame.getFrameDescriptor(frame.LEFT_JAMB_A, verticalJambGeometry), edge, 3, radians(180.0_deg));
		snapSurface(frame.getFrameDescriptor(frame.TOP_JAMB_A, horizontalJambGeometry), surfaces[rightJambIndexA].getEdge(0), 2, radians(90.0_deg));
		snapSurface(frame.getFrameDescriptor(frame.RIGHT_CASING_A, verticalCasingGeometry), surfaces[rightJambIndexA].getEdge(1), 3, radians(-90.0_deg));
		snapSurface(frame.getFrameDescriptor(frame.LEFT_CASING_A, verticalCasingGeometry), surfaces[leftJambIndexA].getEdge(3), 1, radians(90.0_deg));

		edge = subsurfaceA.getEdge(0);

		std::swap(edge.a, edge.b);

		snapSurface(frame.getFrameDescriptor(frame.TOP_CASING_A, horizontalCasingGeometry), edge, 0, 0.0_rad);
		snapSurface(frame.getFrameDescriptor(frame.RIGHT_CASING_B, verticalCasingGeometry), surfaces[rightJambIndexA].getEdge(3), 1, radians(-90.0_deg));
		snapSurface(frame.getFrameDescriptor(frame.LEFT_CASING_B, verticalCasingGeometry), surfaces[leftJambIndexA].getEdge(1), 3, radians(90.0_deg));

		edge = subsurfaceB.getEdge(0);

		std::swap(edge.a, edge.b);

		snapSurface(frame.getFrameDescriptor(frame.TOP_CASING_B, horizontalCasingGeometry), edge, 0, 0.0_rad);
	}
}