#pragma once

#include "NamedValue.h"
#include "AttrList.h"
#include "detail/FieldTraits.h"
#include "Util.h"

namespace Ubpa::USRefl {
	template<typename T, typename AList, typename Char, Char... chars>
	struct Field : detail::FieldTraits<T>, NamedValue<T, Char, chars...> {
		static_assert(detail::IsInstance<AList, AttrList>::value);
		static_assert(!std::is_void_v<T>);

		AList attrs;

		constexpr Field(std::integer_sequence<Char, chars...> name, T value, AList attrs = {})
			: NamedValue<T, Char, chars...>{ name, value }, attrs{ attrs }{}
	};

	template<typename T, typename AList, typename Char, Char... chars>
	Field(std::integer_sequence<Char, chars...>, T, AList)->Field<T, AList, Char, chars...>;
	template<typename T, typename Char, Char... chars>
	Field(std::integer_sequence<Char, chars...>, T)->Field<T, AttrList<>, Char, chars...>;
}
