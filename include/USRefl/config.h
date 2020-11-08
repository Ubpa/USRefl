#pragma once

#ifndef UBPA_USREFL_NOT_USE_NAMEOF
  #define UBPA_USREFL_USE_NAMEOF
#endif

// AutoRefl will use these names
namespace UMeta {
	static constexpr char nameof_namespace[] = "UMeta";

	static constexpr char initializer[] = "UMeta_initializer";
	static constexpr char default_functions[] = "UMeta_default_functions";
	static constexpr char constructor[] = "UMeta_constructor";
	static constexpr char destructor[] = "UMeta_destructor";

	static constexpr char nameof_initializer[] = "initializer";
	static constexpr char nameof_default_functions[] = "default_functions";
	static constexpr char nameof_constructor[] = "constructor";
	static constexpr char nameof_destructor[] = "destructor";
}
