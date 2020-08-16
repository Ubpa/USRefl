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
struct Ubpa::USRefl::TypeInfo<Point>
	: Ubpa::USRefl::TypeInfoBase<Point>
{
	static constexpr AttrList attrs = {
		Attr{"size", 8},
	};

	static constexpr FieldList fields = {
		Field{"x", &Point::x,
			AttrList{
				Attr{"not_serialize"},
			}
		},
		Field{"y", &Point::y,
			AttrList{
				Attr{"info", "hello"},
			}
		},
		Field{"id", &Point::id},
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

	constexpr auto y_idx = TypeInfo<Point>::fields.Find("y");
	constexpr auto y_field = TypeInfo<Point>::fields.Get<y_idx>();
	static_assert(y_field.name == "y");

	static_assert(TypeInfo<Point>::fields.Contains("x"));

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
			cout << field.name << ": " << *field.value << endl;
	});
}
