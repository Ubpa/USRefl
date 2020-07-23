#pragma once

#include "NamedValue.h"
#include "AttrList.h"
#include "detail/FieldTraits.h"
#include "detail/Util.h"

namespace Ubpa::USRefl {
	template<typename T, typename AList>
	struct Field : detail::FieldTraits<T>, NamedValue<T> {
		static_assert(detail::IsInstance<AList, AttrList>::value);
		static_assert(!std::is_void_v<T>);

		AList attrs;

		constexpr Field(std::string_view name, T value, AList attrs = {})
			: NamedValue<T>{ name, value }, attrs{ attrs }{}
	};

	template<typename T, typename AList>
	Field(std::string_view, T, AList)->Field<T, AList>;
	template<typename T>
	Field(std::string_view, T)->Field<T, AttrList<>>;
}
