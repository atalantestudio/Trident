// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class ScriptEntity {
		public:
			/* template<typename T>
			const T& getComponent() const {
				return ecs.getComponent<T>(this);
			} */

			virtual void onConstruct() {}

			virtual void onUpdate() {}

			virtual void onDestroy() {}
	};
}