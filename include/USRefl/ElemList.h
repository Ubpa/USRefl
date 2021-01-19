#pragma once

#include "NamedValue.h"

#include <tuple>

namespace Ubpa::USRefl {
	// Elems is named value
	template<typename... Elems>
	struct ElemList {
		std::tuple<Elems...> elems;
		static constexpr std::size_t size = sizeof...(Elems);

		constexpr ElemList(Elems... elems) : elems{ elems... } {}

		template<typename Acc, typename Func>
		constexpr auto Accumulate(Acc acc, Func&& func) const;

		template<typename Func>
		constexpr void ForEach(Func&& func) const;

		template<typename Func>
		constexpr std::size_t FindIf(Func&& func) const;

		template<typename Name>
		constexpr const auto& Find(Name = {}) const;

		template<typename T>
		constexpr std::size_t FindValue(const T& value) const;

		template<typename T, typename Str>
		constexpr const T* ValuePtrOfName(Str name) const;

		template<typename T, typename Str>
		constexpr const T& ValueOfName(Str name) const { return *ValuePtrOfName<T>(name); }

		template<typename T, typename Char = char>
		constexpr std::basic_string_view<Char> NameOfValue(const T& value) const;

		template<typename Name>
		static constexpr bool Contains(Name = {});

		template<std::size_t N>
		constexpr const auto& Get() const;

		template<typename Elem>
		constexpr auto Push(Elem e) const;

		template<typename Elem>
		constexpr auto Insert(Elem e) const;

#define USRefl_ElemList_GetByValue(list, value) list.Get<list.FindValue(value)>()
	};
}

#include "detail/ElemList.inl"
