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
		Field {USTR("Sum"), static_cast<float(Type::*)()const>(&Type::Sum)},
		Field {USTR("Sum"), static_cast<float(Type::*)(float)const>(&Type::Sum)},
		Field {USTR("id"), Type::id},
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
