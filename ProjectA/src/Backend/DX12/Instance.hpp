// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Instance.hpp"

namespace ProjectA::DX12 {
	class Instance : public ProjectA::Instance {
		public:
			explicit Instance();

		public:
			ComPtr<IDXGIFactory4> factory;
	};
}