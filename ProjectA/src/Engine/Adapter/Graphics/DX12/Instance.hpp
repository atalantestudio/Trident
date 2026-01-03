// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"

namespace ProjectA {
	class DX12::Instance final : public ProjectA::Instance {
		public:
			explicit Instance();

		public:
			ComPtr<IDXGIFactory4> factory;
	};
}