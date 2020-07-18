#pragma once

#include "Base.h"
#include "ElemList.h"
// #include "detail/Util.h"

namespace Ubpa::USRefl {
	template<typename... Bases>
	struct BaseList : ElemList<Bases...> {
		// static_assert((detail::IsInstance<Bases, Base>::value&&...));
		constexpr BaseList(Bases... bases)
			: ElemList<Bases...>{ bases... } {};
	};
}
