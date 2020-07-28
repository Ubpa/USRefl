#include <USRefl/USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;

	float Sum() const {
		return x + y;
	}
	float Sum(float z) const {
		return x + y + z;
	}

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
		Field{"Sum", static_cast<float(Point::*)()const>(&Point::Sum)},
		Field{"Sum", static_cast<float(Point::*)(float)const>(&Point::Sum),
			AttrList {
				Attr{"__arg_0",
					AttrList{
						Attr{"__name", "z"},
					}
				},
			}
		},
	};
};

int main() {
	Point p{ 1,2 };

	TypeInfo<Point>::fields.ForEach([p](auto field) {
		if constexpr (field.is_func) {
			if (field.name != "Sum")
				return;
			if constexpr (field.ValueTypeIsSameWith(static_cast<float(Point::*)()const>(&Point::Sum)))
				cout << (p.*(field.value))() << endl;
			else if constexpr (field.ValueTypeIsSameWith(static_cast<float(Point::*)(float)const>(&Point::Sum)))
				cout << (p.*(field.value))(1.f) << endl;
			else
				assert(false);
		}
	});
}
