// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Serializes the provided object.
	template<typename ObjectType>
	view<char8> serialize(ObjectType object);

	/// Deserializes the provided text.
	template<typename ObjectType>
	ObjectType deserialize(view<char8> text);
}