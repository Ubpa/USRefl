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
			cout << "key   : " << attr.key << endl;
			if constexpr (!attr.is_value_empty)
				cout << "value : " << attr.value << endl;
		});
	});

	constexpr auto y_idx = Type<Point<float>>::fields.Find("y");
	constexpr auto y_field = Type<Point<float>>::fields.Get<y_idx>();
	static_assert(y_field.name == "y");

	static_assert(Type<Point<float>>::fields.Contains("x"));

	Type<Point<float>>::attrs.ForEach([](auto attr) {
		cout << "key   : " << attr.key << endl;
		if constexpr (!attr.is_value_empty)
			cout << "value : " << attr.value << endl;
	});

	ForEachFieldOf(p, [](auto field) {
		cout << field << endl;
	});
}
