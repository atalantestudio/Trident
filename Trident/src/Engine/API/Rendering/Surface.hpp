// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/API/Rendering/Instance.hpp"

namespace Trident {
	class Surface {
		public:
			~Surface();

			ImageFormat getFormat() const {
				return format;
			}

		protected:
			explicit Surface(const Instance* instance);

		public:
			const Instance* instance;
			ImageFormat format;
	};
}