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
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	// [!] all instance types have the same name
	static constexpr char name[6] = "Point";
#endif
	static constexpr AttrList attrs = {
		Attr {USTR("size"), 8},
	};
	static constexpr FieldList fields = {
		Field {USTR("x"), &Point<T>::x, AttrList {
			Attr {USTR("not_serialize")},
		}},
		Field {USTR("y"), &Point<T>::y, AttrList {
			Attr {USTR("info"), "hello"},
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

	constexpr auto y_field = TypeInfo<Point<float>>::fields.Find(USTR("y"));
	static_assert(y_field.name == "y");

	static_assert(TypeInfo<Point<float>>::fields.Contains(USTR("x")));

	TypeInfo<Point<float>>::attrs.ForEach([](auto attr) {
		cout << "name  : " << attr.name << endl;
		if constexpr (attr.has_value)
			cout << "value : " << attr.value << endl;
	});

	TypeInfo<Point<float>>::ForEachVarOf(p, [](auto field, auto&& var) {
		cout << field.name << " : " << var << endl;
	});
}
