#pragma once

namespace Ubpa::USRefl {
	template<typename Name, typename T>
	struct Attr : NamedValue<Name, T> {
		constexpr Attr(Name name, T value)
			: NamedValue<Name, T>{ name, value } {}
	};

	template<typename Name>
	struct Attr<Name, void> : NamedValue<Name, void> {
		constexpr Attr(Name name) : NamedValue<Name, void>{ name } {}
	};
}
