#pragma once

namespace Trident {
	class ScriptEntity {
		public:
			/*template<typename T>
			const T& getComponent() const {
				return ecs.getComponent<T>(this);
			}*/

			virtual void onConstruct() {}

			virtual void onUpdate() {}

			virtual void onDestroy() {}
	};
}