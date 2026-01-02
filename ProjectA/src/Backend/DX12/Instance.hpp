// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"

namespace ProjectA {
	namespace DX12 {
		class Instance;
	}

	class DX12::Instance final : public ProjectA::Instance {
		public:
			explicit Instance();

		public:
			ComPtr<IDXGIFactory4> factory;
	};
}