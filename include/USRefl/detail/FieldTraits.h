#pragma once

#include <type_traits>

namespace Ubpa::USRefl::detail {
	template<typename T>
	struct FieldTraits;

	template<typename Obj, typename Value, bool isStatic>
	struct FieldTraitsBase {
		using object_type = Obj;
		using value_type = Value;
		static constexpr bool is_static = isStatic;
		static constexpr bool is_func = std::is_function_v<Value>;
	};

	// non-static member pointer
	template<typename Object, typename T>
	struct FieldTraits<T Object::*>
		: FieldTraitsBase<Object, T, false> {};

	// static member pointer
	template<typename T>
	struct FieldTraits<T*>
		: FieldTraitsBase<void, T, true> {};

	// enum / static constexpr
	template<typename T>
	struct FieldTraits
		: FieldTraitsBase<void, T, true> {};
}
