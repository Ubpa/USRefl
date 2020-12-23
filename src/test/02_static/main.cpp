#include <USRefl/USRefl.h>

#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;

	static constexpr std::size_t id = 1024;
};

template<>
struct Ubpa::USRefl::TypeInfo<Point> :
	TypeInfoBase<Point>
{
	static constexpr AttrList attrs = {
		Attr {TSTR("size"), 8},
	};
	static constexpr FieldList fields = {
		Field {TSTR("x"), &Type::x, AttrList {
			Attr {TSTR("not_serialize")},
		}},
		Field {TSTR("y"), &Type::y, AttrList {
			Attr {TSTR("info"), "hello"},
		}},
		Field {TSTR("id"), Type::id},
	};
};

int main() {
	Point p{ 1,2 };

	TypeInfo<Point>::fields.ForEach([](auto field) {
		cout << field.name << endl;
		field.attrs.ForEach([](auto attr) {
			cout << "name   : " << attr.name << endl;
			if constexpr (attr.has_value)
				cout << "value : " << attr.value << endl;
		});
	});

	constexpr auto y_field = TypeInfo<Point>::fields.Find(TSTR("y"));
	static_assert(y_field.name == "y");

	static_assert(TypeInfo<Point>::fields.Contains(TSTR("x")));

	TypeInfo<Point>::attrs.ForEach([](auto attr) {
		cout << "name   : " << attr.name << endl;
		if constexpr (!attr.has_value)
			cout << "value : " << attr.value << endl;
	});

	TypeInfo<Point>::ForEachVarOf(p, [](auto field, auto&& var) {
		cout << field.name << " : " << var << endl;
	});

	TypeInfo<Point>::fields.ForEach([](auto field) {
		if constexpr (field.is_static)
			cout << field.name << ": " << field.value << endl;
	});
}
