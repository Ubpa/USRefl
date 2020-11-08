#pragma once

#ifndef UBPA_TSTR
#define UBPA_TSTR

#include "detail/TStr.inl"

#include <string_view>

// [C-style string type (value)]
// in C++20, we can easily put a string into template parameter list
// but in C++17, we just can use this disgusting trick
#define TSTR(s)                                                           \
(Ubpa::detail::TSTRHelper([] {                                            \
    struct tmp { static constexpr decltype(auto) get() { return (s); } }; \
    return tmp{};                                                         \
}()))

namespace Ubpa {
	template<typename Char_, Char_... chars>
	struct TStr {
		using Tag = TStr;
		using Char = Char_;
		template<typename T>
		static constexpr bool NameIs(T = {}) noexcept { return std::is_same_v<T, Tag>; }
		static constexpr char name_data[]{ chars...,Char(0) };
		static constexpr std::string_view name{ name_data };
	};
	
	template<typename T>
	struct IsTStr : std::false_type {};
	
	template<typename Char, Char... chars>
	struct IsTStr<TStr<Char, chars...>> : std::true_type {};
}

#endif // UBPA_TSTR
