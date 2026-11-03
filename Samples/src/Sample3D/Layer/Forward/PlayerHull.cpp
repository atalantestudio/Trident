// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Sample3D/Layer/Forward/ForwardLayer.hpp"

namespace Trident {
	void Sample3DForwardLayer::createPlayerHullData(ref<Scene3DAsset> sceneAsset) {
		constexpr float32 x = PLAYER_SCALE.x;
		constexpr float32 y = PLAYER_SCALE.y;
		constexpr float32 z = PLAYER_SCALE.z;

		playerHullVertices = {
			{ 0.5f * x,  0.5f * y,  0.5f * z},
			{-0.5f * x,  0.5f * y,  0.5f * z},
			{ 0.5f * x, -0.5f * y,  0.5f * z},
			{-0.5f * x, -0.5f * y,  0.5f * z},
			{-0.5f * x,  0.5f * y, -0.5f * z},
			{ 0.5f * x,  0.5f * y, -0.5f * z},
			{-0.5f * x, -0.5f * y, -0.5f * z},
			{ 0.5f * x, -0.5f * y, -0.5f * z},
		};
		playerHullEdges = {
			{.originVertexIndex = 0, .faceIndex = 0},
			{.originVertexIndex = 1, .faceIndex = 4},
			{.originVertexIndex = 1, .faceIndex = 0},
			{.originVertexIndex = 3, .faceIndex = 2},
			{.originVertexIndex = 3, .faceIndex = 0},
			{.originVertexIndex = 2, .faceIndex = 5},
			{.originVertexIndex = 2, .faceIndex = 0},
			{.originVertexIndex = 0, .faceIndex = 3},
			{.originVertexIndex = 4, .faceIndex = 1},
			{.originVertexIndex = 5, .faceIndex = 4},
			{.originVertexIndex = 5, .faceIndex = 1},
			{.originVertexIndex = 7, .faceIndex = 3},
			{.originVertexIndex = 7, .faceIndex = 1},
			{.originVertexIndex = 6, .faceIndex = 5},
			{.originVertexIndex = 6, .faceIndex = 1},
			{.originVertexIndex = 4, .faceIndex = 2},
			{.originVertexIndex = 0, .faceIndex = 4},
			{.originVertexIndex = 5, .faceIndex = 3},
			{.originVertexIndex = 4, .faceIndex = 4},
			{.originVertexIndex = 1, .faceIndex = 2},
			{.originVertexIndex = 7, .faceIndex = 5},
			{.originVertexIndex = 2, .faceIndex = 3},
			{.originVertexIndex = 3, .faceIndex = 5},
			{.originVertexIndex = 6, .faceIndex = 2},
		};
		playerHullFaceEdgeIndices = {
			0, 2, 4, 6,
			8, 10, 12, 14,
			19, 15, 23, 3,
			17, 7, 21, 11,
			9, 18, 1, 16,
			5, 22, 13, 20,
		};
		playerHullFaces = {
			{{ 0.0f,  0.0f,  1.0f}, z * 0.5f, {0, 4}},
			{{ 0.0f,  0.0f, -1.0f}, z * 0.5f, {4, 4}},
			{{-1.0f,  0.0f,  0.0f}, x * 0.5f, {8, 4}},
			{{ 1.0f,  0.0f,  0.0f}, x * 0.5f, {12, 4}},
			{{ 0.0f,  1.0f,  0.0f}, y * 0.5f, {16, 4}},
			{{ 0.0f, -1.0f,  0.0f}, y * 0.5f, {20, 4}},
		};

		constexpr uint16 playerHullLineIndices[] = {
			0, 1,
			1, 3,
			3, 2,
			2, 0,
			4, 5,
			5, 7,
			7, 6,
			6, 4,
			0, 5,
			4, 1,
			7, 2,
			3, 6,
		};

		sequence<vec3<float32>> hullLineVertices(sceneAsset->hullLineVertices.count() + 8);
		sequence<uint16> hullLineIndices(sceneAsset->hullLineIndices.count() + 24);

		std::copy(sceneAsset->hullLineVertices.begin(), sceneAsset->hullLineVertices.end(), &hullLineVertices[0]);
		std::copy(sceneAsset->hullLineIndices.begin(), sceneAsset->hullLineIndices.end(), &hullLineIndices[0]);

		std::copy(playerHullVertices.begin(), playerHullVertices.end(), &hullLineVertices[sceneAsset->hullLineVertices.count()]);
		std::copy_n(playerHullLineIndices, 24, &hullLineIndices[sceneAsset->hullLineIndices.count()]);

		sceneAsset->hullLineVertices = hullLineVertices;
		sceneAsset->hullLineIndices = hullLineIndices;
	}
}