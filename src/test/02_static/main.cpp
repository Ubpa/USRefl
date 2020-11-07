#include <USRefl/USRefl.h>

#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;

	static constexpr size_t id = 1024;
};

template<>
struct Ubpa::USRefl::TypeInfo<Point> :
	TypeInfoBase<Point>
{
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	static constexpr char name[6] = "Point";
#endif
	static constexpr AttrList attrs = {
		Attr {USTR("size"), 8},
	};
	static constexpr FieldList fields = {
		Field {USTR("x"), &Type::x, AttrList {
			Attr {USTR("not_serialize")},
		}},
		Field {USTR("y"), &Type::y, AttrList {
			Attr {USTR("info"), "hello"},
		}},
		Field {USTR("id"), Type::id},
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

	constexpr auto y_field = TypeInfo<Point>::fields.Find(USTR("y"));
	static_assert(y_field.name == "y");

	static_assert(TypeInfo<Point>::fields.Contains(USTR("x")));

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
