#pragma once

namespace Ubpa::USRefl {
	template<typename Signature>
	constexpr auto WrapConstructor();
	template<typename T>
	constexpr auto WrapDestructor();

	namespace Name {
		static constexpr const char constructor[] = "__constructor";
		static constexpr const char destructor[] = "__destructor";
		static constexpr const char default_value[] = "__default_value"; // function argument default value
		static constexpr const char name[] = "__name"; // function argument name
		// compile time
        #define UBPA_USREFL_NAME_ARG(x) "__arg_" #x
	}
}

#include "detail/Util.inl"
