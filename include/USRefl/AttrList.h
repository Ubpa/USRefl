#pragma once

#include "ElemList.h"

#include "Attr.h"

namespace Ubpa::USRefl {
	template<typename... Attrs>
	struct AttrList : ElemList<Attrs...> {
		//static_assert((detail::IsInstance<Attrs, Attr>::value&&...));

		constexpr AttrList(Attrs... attrs)
			: ElemList<Attrs...>{ attrs... } {}
	};
}
