#pragma once

#include "NamedValue.h"
#include <string_view>

namespace Ubpa::USRefl {
	// [summary]
	// attribute for field, class, enum
	// [member]
	// T value (T == void -> no value)
	template<typename Name, typename T>
	struct Attr;

	template<typename Name, typename Char, size_t N>
	Attr(Name, const Char(&)[N])->Attr<Name, std::basic_string_view<Char>>;

	template<typename Name>
	Attr(Name)->Attr<Name, void>;
}

#include "detail/Attr.inl"
