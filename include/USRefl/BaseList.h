#pragma once

#include "Base.h"
#include "ElemList.h"

namespace Ubpa::USRefl {
	template<typename... Bases>
	struct BaseList : ElemList<Bases...> {
		constexpr BaseList(Bases... bases)
			: ElemList<Bases...>{ bases... } {};
	};
}
