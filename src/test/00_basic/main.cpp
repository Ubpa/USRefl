#include <USRefl/USRefl.h>

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
struct Ubpa::USRefl::TypeInfo<Point>
	: Ubpa::USRefl::TypeInfoBase<Point>
{
	inline static AttrList attrs = {
		Attr {"size", 8 },
	};
	inline static FieldList fields = {
		Field {"x", &Point::x,
			AttrList {
				Attr {"not_serialize" },
			}
		},
		Field {"y", &Point::y,
			AttrList {
				Attr {"info", "hello" },
			}
		},
	};
};

int main() {
	cout << TypeInfo<Point>::name << endl;

	TypeInfo<Point>::fields.ForEach([](auto field) {
		cout << field.name << endl;
		field.attrs.ForEach([](auto attr) {
			cout << attr.name;
			if constexpr (attr.has_value)
				cout << " : " << attr.value;
			cout << endl;
		});
	});
}
