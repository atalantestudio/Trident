// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Instance.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	class GL::Instance final : public Trident::Instance {
		public:
			explicit Instance(const Trident::GraphicsAdapter* adapter);
	};

	namespace GL {
		ref<Trident::Instance> createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor);

		void destroyInstance(const Trident::Instance* instance);
	}
}