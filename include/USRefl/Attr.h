#pragma once

#include "NamedValue.h"

namespace Ubpa::USRefl {
	// [summary]
	// attribute for field, class, enum
	// [member]
	// std::string_view name
	// T value (T == void -> no value)
	template<typename T>
	struct Attr;

	template<size_t N>
	Attr(std::string_view, const char(&)[N])->Attr<std::string_view>;

	Attr(std::string_view)->Attr<void>;
}

#include "detail/Attr.inl"
