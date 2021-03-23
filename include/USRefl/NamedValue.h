#pragma once

#include <UTemplate/TStr.hpp>

#include <string_view>

namespace Ubpa::USRefl {
	template<typename Name, typename T>
	struct NamedValue;

	template<typename Name, typename T>
	struct NamedValueBase {
		using TName = Name;
		static constexpr std::string_view name = Name::View();
		static constexpr bool has_value = !std::is_void_v<T>;

		template<typename Str>
		static constexpr bool NameIs(Str = {}) noexcept {
			return std::is_same_v<Str, Name>;
		}

		template<typename U>
		static constexpr bool ValueTypeIs() noexcept {
			return std::is_same_v<T, U>;
		}

		template<typename U>
		static constexpr bool ValueTypeIsSameWith(U) noexcept {
			return ValueTypeIs<U>();
		}
	};

	template<typename Name, typename T>
	struct NamedValue : NamedValueBase<Name, T> {
		T value;

		constexpr NamedValue(Name, T value) : value{ value } {}

		template<typename U>
		constexpr bool operator==(U v) const {
			if constexpr (std::is_same_v<T, U>)
				return value == v;
			else
				return false;
		}
	};

	template<typename Name>
	struct NamedValue<Name, void> : NamedValueBase<Name, void> {
		constexpr NamedValue(Name){}
		template<typename U>
		constexpr bool operator==(U) const { return false; }
	};

	template<typename Name>
	NamedValue(Name)->NamedValue<Name, void>;
	template<typename T, typename Name>
	NamedValue(Name, T)->NamedValue<Name, T>;
}
