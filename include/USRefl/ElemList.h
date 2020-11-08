#pragma once

#include "NamedValue.h"

#include <tuple>

namespace Ubpa::USRefl {
	// Elems is named value
	template<typename... Elems>
	struct ElemList {
		std::tuple<Elems...> elems;
		static constexpr size_t size = sizeof...(Elems);

		constexpr ElemList(Elems... elems) : elems{ elems... } {}

		template<bool... masks, typename Acc, typename Func>
		constexpr auto Accumulate(Acc&& acc, Func&& func) const;

		template<bool... masks, typename Func>
		constexpr void ForEach(Func&& func) const;

		template<typename Func>
		constexpr size_t FindIf(Func&& func) const;

		template<typename Name>
		constexpr auto Find(Name = {}) const;

		template<typename T>
		constexpr size_t FindValue(T value) const;

		template<typename T, typename Char>
		constexpr T ValueOfName(std::basic_string_view<Char> name) const;

		template<typename T>
		constexpr T ValueOfName(std::string_view name) const { return ValueOfName<T, char>(name); }

		template<typename T>
		constexpr T ValueOfName(std::wstring_view name) const { return ValueOfName<T, wchar_t>(name); }

		template<typename T, typename Char = char>
		constexpr std::basic_string_view<Char> NameOfValue(T value) const;

		template<typename Name>
		static constexpr bool Contains(Name = {});

		template<size_t N>
		constexpr auto Get() const;

		template<typename Elem>
		constexpr auto Push(Elem e) const;

		template<typename Elem>
		constexpr auto Insert(Elem e) const;

// value must be constexpr
// C++20 support string literal as template arguments
#define USRefl_ElemList_GetByValue(list, value) list.Get<list.FindValue(value)>()
	};
}

#include "detail/ElemList.inl"
