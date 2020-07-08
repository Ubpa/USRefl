#include <USRefl.h>

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
struct Type<Point<T>> {
	static constexpr std::string_view name = "Point"; // use nameof
	using type = Point;

	static constexpr FieldList fields = {
		Field{"x", &Point<T>::x, AttrList{ Attr{ "not_serialize", true } }},
		Field{"y", &Point<T>::y, AttrList{ Attr{ "info", "hello" } }}
	};

	static constexpr AttrList attrs = {
		Attr{ "size", 8 }
	};
};

int main() {
	Point<float> p{ 1,2 };

	Type<Point<float>>::fields.ForEach([](auto field) {
		cout << field.name << endl;
		field.attrs.ForEach([](auto attr) {
			cout << "name   : " << attr.name << endl;
			if constexpr (attr.has_value)
				cout << "value : " << attr.value << endl;
		});
	});

	constexpr auto y_idx = Type<Point<float>>::fields.Find("y");
	constexpr auto y_field = Type<Point<float>>::fields.Get<y_idx>();
	static_assert(y_field.name == "y");

	static_assert(Type<Point<float>>::fields.Contains("x"));

	Type<Point<float>>::attrs.ForEach([](auto attr) {
		cout << "name   : " << attr.name << endl;
		if constexpr (attr.has_value)
			cout << "value : " << attr.value << endl;
	});

	ForEachVarOf(p, [](auto&& var) {
		cout << var << endl;
	});
}
