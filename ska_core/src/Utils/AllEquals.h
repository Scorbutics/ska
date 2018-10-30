#pragma once

namespace ska {

	inline bool AllEquals() {
		return true;
	}

	template <class Object1, class Object2>
	bool AllEquals(Object1&& obj1, Object2&& obj2) {
		return obj1 == obj2;
	}
	

	template <class CurrentObject1, class CurrentObject2, class ... Objects>
	bool AllEquals(CurrentObject1&& currentObject1, CurrentObject2&& currentObject2, Objects&& ... obj) {
		return AllEquals(currentObject1, currentObject2) 
			&& AllEquals(currentObject2, std::forward<Objects>(obj)...);
	}
}
