#pragma once

#include "NamedValue.h"
#include <string_view>

namespace Ubpa::USRefl {
	// [summary]
	// attribute for field, class, enum
	// [member]
	// std::string_view name
	// T value (T == void -> no value)
	template<typename T, typename Char, Char... chars>
	struct Attr;

	template<size_t N, typename Char, Char... chars>
	Attr(std::integer_sequence<Char, chars...>, const char(&)[N])->Attr<std::string_view, Char, chars...>;

	template<typename Char, Char... chars>
	Attr(std::integer_sequence<Char, chars...>)->Attr<void, Char, chars...>;
}

#include "detail/Attr.inl"
