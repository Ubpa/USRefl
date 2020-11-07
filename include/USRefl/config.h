#pragma once

#ifndef UBPA_USREFL_NOT_USE_NAMEOF
  #define UBPA_USREFL_USE_NAMEOF
#endif

// AutoRefl will use these names
namespace _USRefl {
	static constexpr char nameof_namespace[] = "_USRefl";

	static constexpr char initializer[] = "_USRefl_initializer";
	static constexpr char default_functions[] = "_USRefl_default_functions";
	static constexpr char constructor[] = "_USRefl_constructor";
	static constexpr char destructor[] = "_USRefl_destructor";

	static constexpr char nameof_initializer[] = "initializer";
	static constexpr char nameof_default_functions[] = "default_functions";
	static constexpr char nameof_constructor[] = "constructor";
	static constexpr char nameof_destructor[] = "destructor";
}
