#pragma once

#include "detail/NamedValue.inl"

#include <string_view>

// [USRefl static C-style string]
// std::integer_sequence<Char, chars...>
// in C++20, we can easily put a string into template parameter list
// but in C++17, we just can use this disgusting trick
#define USTR(s)                                                           \
(Ubpa::USRefl::detail::USTRHelper([] {                                    \
    struct tmp { static constexpr decltype(auto) get() { return (s); } }; \
    return tmp{};                                                         \
}()))

namespace Ubpa::USRefl {
	template<typename T, typename Char, Char... chars>
	struct NamedValue;

	template<typename T, typename Char, Char... chars>
	struct NamedValueBase {
		using NameTag = std::integer_sequence<Char, chars...>;
		static constexpr char name_data[]{ chars..., static_cast<Char>(0) };
		static constexpr std::basic_string_view<Char> name{ name_data };
		static constexpr bool has_value = !std::is_void_v<T>;

		template<typename U>
		static constexpr bool ValueTypeIs() {
			return std::is_same_v<T, U>;
		}

		template<typename U>
		static constexpr bool ValueTypeIsSameWith(U) {
			return ValueTypeIs<U>();
		}
	};

	template<typename T, typename Char, Char... chars>
	struct NamedValue : NamedValueBase<T, Char, chars...> {
		T value;

		constexpr NamedValue(std::integer_sequence<Char, chars...>, T value)
			: value{ value } {}

		template<typename U>
		constexpr bool operator==(U v) const {
			if constexpr (std::is_same_v<T, U>)
				return value == v;
			else
				return false;
		}
	};

	template<typename Char, Char... chars>
	struct NamedValue<void, Char, chars...> : NamedValueBase<void, Char, chars...> {
		constexpr NamedValue(std::integer_sequence<Char, chars...>) {}

		template<typename U>
		constexpr bool operator==(U) const { return false; }
	};

	template<typename Char, Char... chars>
	NamedValue(std::integer_sequence<Char, chars...>)->NamedValue<void, Char, chars...>;
	template<typename T, typename Char, Char... chars>
	NamedValue(std::integer_sequence<Char, chars...>, T)->NamedValue<T, Char, chars...>;
}
