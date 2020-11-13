#pragma once

#include <type_traits>

namespace Ubpa::USRefl::detail {
	template<typename T>
	struct FieldTraits;

	template<typename Obj, typename Value, bool isStatic, bool isFunction>
	struct FieldTraitsBase {
		using object_type = Obj;
		using value_type = Value;
		static constexpr bool is_static = isStatic;
		static constexpr bool is_func = isFunction;
	};

	// non-static member pointer
	template<typename Object, typename T>
	struct FieldTraits<T Object::*>
		: FieldTraitsBase<Object, T, false, std::is_function_v<T>> {};

	// static member pointer
	template<typename T>
	struct FieldTraits<T*>
		: FieldTraitsBase<void, T, true, std::is_function_v<T>> {};

	// enum / static constexpr
	template<typename T>
	struct FieldTraits
		: FieldTraitsBase<void, T, true, false> {};
}
