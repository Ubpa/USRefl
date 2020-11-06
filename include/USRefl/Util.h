#pragma once

namespace Ubpa::USRefl {
	template<typename Signature>
	constexpr auto WrapConstructor();
	template<typename T>
	constexpr auto WrapDestructor();

	namespace Name {
		static constexpr char constructor[] = "__constructor";
		static constexpr char destructor[] = "__destructor";
		static constexpr char default_value[] = "__default_value";
		static constexpr char name[] = "__name"; // function argument name
		// compile time
        #define UBPA_USREFL_NAME_ARG(x) "__arg_" #x
	}
}

#include "detail/Util.inl"
