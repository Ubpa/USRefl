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

		template<typename Func, typename Acc>
		constexpr auto Accumulate(Acc&& acc, Func&& func) const;

		template<typename Func>
		constexpr void ForEach(Func&& func) const;

		template<typename Func>
		constexpr size_t FindIf(Func&& func) const;

		constexpr size_t Find(std::string_view name) const;

		template<typename T>
		constexpr size_t FindValue(T value) const;

		constexpr bool Contains(std::string_view name) const;

		template<size_t N>
		constexpr auto Get() const;

		template<typename Elem>
		constexpr auto UniqueInsert(Elem e) const;

// name must be constexpr std::string_view / const char[N]
// C++20 support string literal as template arguments
#define USRefl_ElemList_GetByName(list, name) list.Get<list.Find(name)>()

// value must be constexpr
// C++20 support string literal as template arguments
#define USRefl_ElemList_GetByValue(list, value) list.Get<list.FindValue(value)>()
	};
}

#include "detail/ElemList.inl"
