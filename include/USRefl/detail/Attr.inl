#pragma once

namespace Ubpa::USRefl {
	template<typename T, typename Char, Char... chars>
	struct Attr : NamedValue<T, Char, chars...> {
		constexpr Attr(std::integer_sequence<Char, chars...> name, T value)
			: NamedValue<T, Char, chars...>{ name, value } {}
	};

	template<typename Char, Char... chars>
	struct Attr<void, Char, chars...> : NamedValue<void, Char, chars...> {
		constexpr Attr(std::integer_sequence<Char, chars...> name) : NamedValue<void, Char, chars...>{ name } {}
	};
}
