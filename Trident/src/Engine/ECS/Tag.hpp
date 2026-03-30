// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	#define DEFINE_TAG_COMPONENT(tag) struct tag {}

	DEFINE_TAG_COMPONENT(DynamicEntityTag);
	DEFINE_TAG_COMPONENT(StaticEntityTag);
}