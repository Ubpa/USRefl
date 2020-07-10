#include <USRefl.h>

#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
};

template<>
struct TypeInfo<Point> : TypeInfoBase<Point> {
	static constexpr std::string_view name = "Point";

	static constexpr FieldList fields = {
		Field{"x", &Point::x, AttrList{ Attr{ "not_serialize" } }},
		Field{"y", &Point::y, AttrList{ Attr{ "info", "hello" } }}
	};

	static constexpr AttrList attrs = {
		Attr{ "size", 8 }
	};
};

int main() {
	Point p{ 1,2 };

	TypeInfo<Point>::fields.ForEach([](auto field) {
		cout << field.name << endl;
		field.attrs.ForEach([](auto attr) {
			cout << attr.name << endl;
			if constexpr (attr.has_value)
				cout << ": " << attr.value << endl;
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

	TypeInfo<Point>::DFS_ForEachVarOf(p, [](auto&& var) {
		cout << var << endl;
	});
}
