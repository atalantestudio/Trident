#pragma once

#include "ProjectA/InputInterface.hpp"

namespace ProjectA {
	class Input : public InputInterface {
		protected:
			bool isKeyPressedImpl(int keyCode) final override;

			bool isMouseButtonPressedImpl(int button) final override;

			vec2<float> getMousePositionImpl() final override;
	};
}