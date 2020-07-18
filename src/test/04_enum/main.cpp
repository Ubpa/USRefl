#include <USRefl/USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

template<size_t N>
constexpr size_t Func() { return N; }

enum class [[enum_attr("enum_attr_value")]] Color {
	RED [[enumerator_attr("enumerator_attr_value"), func(&Func<1>)]] = 1,
	GREEN [[func(&Func<2>)]] = 2,
	BLUE [[func(&Func<3>)]] = 4
};

template<>
struct TypeInfo<Color> : TypeInfoBase<Color> {
	static constexpr FieldList fields = {
		Field{"RED", Color::RED, AttrList{ Attr{ "enumerator_attr", "enumerator_attr_value" },Attr{"func", &Func<1>} }},
		Field{"GREEN", Color::GREEN, AttrList{ Attr{"func", &Func<2>} }},
		Field{"BLUE", Color::BLUE, AttrList{ Attr{"func", &Func<3>} }}
	};

	static constexpr AttrList attrs = {
		Attr{ "enum_attr", "enum_attr_value" }
	};
};

int main() {
	cout << TypeInfo<Color>::name << endl;

	TypeInfo<Color>::fields.ForEach([](auto field) {
		cout << field.name << endl;
	});

	static_assert(USRefl_ElemList_GetByName(TypeInfo<Color>::fields, "RED").value == Color::RED);
	static_assert(USRefl_ElemList_GetByValue(TypeInfo<Color>::fields, Color::RED).name == "RED");

	constexpr Color c = Color::GREEN;
	constexpr auto c_attr = USRefl_ElemList_GetByValue(TypeInfo<Color>::fields, c).attrs;
	static_assert(USRefl_ElemList_GetByName(c_attr, "func").value() == 2);
}
