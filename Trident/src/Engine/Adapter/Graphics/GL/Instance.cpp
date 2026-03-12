// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/procedures.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Instance::Instance(const Trident::GraphicsAdapter* adapter) :
		Trident::Instance(adapter)
	{}

	ref<Trident::Instance> GL::createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Instance> instance = createRef<GL::Instance>(adapter);

		return instance;
	}

	void GL::destroyInstance(const Trident::Instance* instance) {}
}