#include <USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

enum class [[enum_attr("enum_attr_value")]] Color {
	RED [[enumerator_attr("enumerator_attr_value")]] = 1,
	GREEN = 2,
	BLUE = 4
};

template<>
struct Type<Color> {
	static constexpr std::string_view name = "Color";

	static constexpr FieldList fields = {
		Field{"RED", Color::RED, AttrList{ Attr{ "enumerator_attr", "enumerator_attr_value" } }},
		Field{"GREEN", Color::GREEN, AttrList{ }},
		Field{"BLUE", Color::BLUE, AttrList{ }}
	};

	static constexpr AttrList attrs = {
		Attr{ "enum_attr", "enum_attr_value" }
	};
};

int main() {
	Type<Color>::fields.ForEach([](auto field) {
		cout << field.name << endl;
	});
	static_assert(Type<Color>::fields.Get<Type<Color>::fields.Find("RED")>().value == Color::RED);
}
