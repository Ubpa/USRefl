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
struct Ubpa::USRefl::TypeInfo<Point<T>>
	: Ubpa::USRefl::TypeInfoBase<Point<T>>
{
	static constexpr AttrList attrs = {
		Attr{"size", 8},
	};

	static constexpr FieldList fields = {
		Field{"x", &Point<T>::x,
			AttrList {
				Attr {"not_serialize" },
			}
		},
		Field{"y", &Point<T>::y,
			AttrList {
				Attr {"info", "hello" },
			}
		},
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

	constexpr auto y_idx = TypeInfo<Point<float>>::fields.Find("y");
	constexpr auto y_field = TypeInfo<Point<float>>::fields.Get<y_idx>();
	static_assert(y_field.name == "y");

	static_assert(TypeInfo<Point<float>>::fields.Contains("x"));

	TypeInfo<Point<float>>::attrs.ForEach([](auto attr) {
		cout << "name  : " << attr.name << endl;
		if constexpr (attr.has_value)
			cout << "value : " << attr.value << endl;
	});

	TypeInfo<Point<float>>::ForEachVarOf(p, [](auto&& var) {
		cout << var << endl;
	});
}
