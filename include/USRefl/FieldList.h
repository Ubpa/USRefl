#pragma once

#include "Field.h"
#include "ElemList.h"
#include "Util.h"

namespace Ubpa::USRefl {
	// Field's (name, value_type) must be unique
	template<typename... Fields>
	struct FieldList : ElemList<Fields...> {
		//static_assert((detail::IsInstance<Fields, Field>::value&&...));
		constexpr FieldList(Fields... fields) : ElemList<Fields...>{ fields... } {};
	};
}
