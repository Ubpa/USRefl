#pragma once

#include <type_traits>

namespace Ubpa::USRefl::detail {
	template<typename T>
	struct FieldTraits;

	// non-static member pointer
	template<typename Object, typename T>
	struct FieldTraits<T Object::*> {
		using object_type = Object;
		using value_type = T;
		static constexpr bool is_static = false;
		static constexpr bool is_func = std::is_function_v<T>;
	};

	// static member pointer
	template<typename T>
	struct FieldTraits<T*> {
		using object_type = void;
		using value_type = T;
		static constexpr bool is_static = true;
		static constexpr bool is_func = std::is_function_v<T>;
	};

	// enum pointer
	template<typename T>
	struct FieldTraits {
		static_assert(std::is_enum_v<T>);
		using object_type = void;
		using value_type = T;
		static constexpr bool is_static = true;
		static constexpr bool is_func = false;
	};
}
