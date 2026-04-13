// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// See https://en.wikipedia.org/wiki/List_of_moments_of_inertia#Moments_of_inertia.
	enum class MomentOfInertia2D : uint8 {
		/// A point of mass m.
		POINT,

		/// A thin solid disk of radius r and mass m.
		THIN_SOLID_DISK,

		/// A thin rectangular plate of width w, height h and mass m.
		/// The axis of rotation is at the center.
		THIN_RECTANGULAR_PLATE,
	};

	/// See https://en.wikipedia.org/wiki/List_of_moments_of_inertia#List_of_3D_inertia_tensors
	enum class MomentOfInertia3D : uint8 {
		/// A solid cuboid of width w (X), height h (Y), depth d (Z) and mass m.
		SOLID_CUBOID,
	};
}