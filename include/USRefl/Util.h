#pragma once

namespace Ubpa::USRefl {
	template<typename Signature>
	constexpr auto WrapConstructor();
	template<typename T>
	constexpr auto WrapDestructor();
}

#include "detail/Util.inl"
