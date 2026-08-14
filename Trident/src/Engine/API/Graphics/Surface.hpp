// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageFormat.hpp"
#include "Engine/API/Graphics/Instance.hpp"

namespace Trident {
	class Surface {
		public:
			Surface() = default;

			~Surface();

			/// Returns the format of the surface.
			ImageFormat getFormat() const;

		protected:
			explicit Surface(const Instance* instance);

		public:
			const Instance* instance;
			ImageFormat format;
	};
}