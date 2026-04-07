// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	static constexpr uint32 CLUSTER_TRIANGLE_COUNT = 128;
	static constexpr uint32 CLUSTER_INDEX_COUNT = CLUSTER_TRIANGLE_COUNT * 3;

	struct Cluster {
		uint32 instanceIndex;
	};
}