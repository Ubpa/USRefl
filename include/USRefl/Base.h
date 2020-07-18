#pragma once

#include "TypeInfo.h"

namespace Ubpa::USRefl {
	template<typename T, bool IsVirtual = false>
	struct Base {
		static constexpr auto info = TypeInfo<T>{};
		static constexpr bool is_virtual = IsVirtual;
	};
}
