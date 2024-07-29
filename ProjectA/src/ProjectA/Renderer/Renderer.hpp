#pragma once

namespace ProjectA {
	enum class GraphicsAPI {
		None = 0,
		OpenGL = 1,
	};

	class Renderer {
		private:
			static GraphicsAPI graphicsAPI;

		public:
			static inline GraphicsAPI getGraphicsAPI() {
				return graphicsAPI;
			}
	};
}