#include <USRefl/USRefl.h>

#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
	static constexpr size_t dim = 2;
};

template<>
struct Ubpa::USRefl::TypeInfo<Point>
	: TypeInfoBase<Point>
{
#ifdef UBPA_USREFL_NOT_USE_NAMEOF
	static constexpr char name[6] = "Point";
#endif
	static constexpr AttrList attrs = {
		Attr {"size", 8},
	};
	static constexpr FieldList fields = {
		Field {"x", &Type::x,
			AttrList {
				Attr {"not_serialize"},
			}
		},
		Field {"y", &Type::y,
			AttrList {
				Attr {"info", "hello"},
			}
		},
		Field {"dim", Type::dim},
	};
};

int main() {
	cout << TypeInfo<Point>::name << endl;

	TypeInfo<Point>::fields.ForEach([](auto field) {
		cout << field.name << ", " << field.value << endl;
		field.attrs.ForEach([](auto attr) {
			cout << attr.name;
			if constexpr (attr.has_value)
				cout << " : " << attr.value;
			cout << endl;
		});
	});
}
