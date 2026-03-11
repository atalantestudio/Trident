// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/config.hpp"

extern "C" {
	#if !defined(GL_PREFER_INTEGRATED_GPU)
		// See https://gpuopen.com/learn/amdpowerxpressrequesthighperformance.
		__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;

		// See https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm.
		__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	#endif
}