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

			ref<Instance> getInstance() const {
				return instance;
			}

		protected:
			explicit Surface(const Instance* instance);

		protected:
			ref<Instance> instance;
			ImageFormat format;
	};
}