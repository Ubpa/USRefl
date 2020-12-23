#include <USRefl/USRefl.h>

#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

template<typename T>
struct [[size(8)]] Point {
	[[not_serialize]]
	T x;
	[[info("hello")]]
	T y;
};

template<typename T>
struct Ubpa::USRefl::TypeInfo<Point<T>> :
	TypeInfoBase<Point<T>>
{
	static constexpr AttrList attrs = {
		Attr {TSTR("size"), 8},
	};
	static constexpr FieldList fields = {
		Field {TSTR("x"), &Point<T>::x, AttrList {
			Attr {TSTR("not_serialize")},
		}},
		Field {TSTR("y"), &Point<T>::y, AttrList {
			Attr {TSTR("info"), "hello"},
		}},
	};
};

int main() {
	Point<float> p{ 1,2 };

	TypeInfo<Point<float>>::fields.ForEach([](auto field) {
		cout << field.name << endl;
		field.attrs.ForEach([](auto attr) {
			cout << "name  : " << attr.name << endl;
			if constexpr (attr.has_value)
				cout << "value : " << attr.value << endl;
		});
	});

	constexpr auto y_field = TypeInfo<Point<float>>::fields.Find(TSTR("y"));
	static_assert(y_field.name == "y");

	static_assert(TypeInfo<Point<float>>::fields.Contains(TSTR("x")));

	TypeInfo<Point<float>>::attrs.ForEach([](auto attr) {
		cout << "name  : " << attr.name << endl;
		if constexpr (attr.has_value)
			cout << "value : " << attr.value << endl;
	});

	TypeInfo<Point<float>>::ForEachVarOf(p, [](auto field, auto&& var) {
		cout << field.name << " : " << var << endl;
	});
}
