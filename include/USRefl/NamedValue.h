#pragma once

#include <string_view>

namespace Ubpa::USRefl {
	template<typename T>
	struct NamedValue;

	template<typename T>
	struct NamedValueBase {
		std::string_view name;
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

	template<typename T>
	struct NamedValue : NamedValueBase<T> {
		T value;

		constexpr NamedValue(std::string_view name, T value)
			: NamedValueBase<T>{ name }, value{ value } {}

		template<typename U>
		constexpr bool operator==(U v) const {
			if constexpr (std::is_same_v<T, U>)
				return value == v;
			else
				return false;
		}
	};

	template<>
	struct NamedValue<void> : NamedValueBase<void> {
		constexpr NamedValue(std::string_view name)
			: NamedValueBase<void>{ name } {}

		template<typename U>
		constexpr bool operator==(U) const { return false; }
	};
}
