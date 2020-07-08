#include <USRefl.h>

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
struct Type<Point> {
	static constexpr std::string_view name = "Point";

	static constexpr FieldList fields = {
		Field{"x", &Point::x, AttrList{ Attr{ "not_serialize", true } }},
		Field{"y", &Point::y, AttrList{ Attr{ "info", "hello" } }},
		Field{"Sum", overload_v<>(&Point::Sum), AttrList{}},
		Field{"Sum", overload_v<float>(&Point::Sum), AttrList{}}
	};

	static constexpr AttrList attrs = {
		Attr{ "size", 8 }
	};
};

int main() {
	Point p{ 1,2 };

	Type<Point>::fields.ForEach([p](auto field) {
		if constexpr (field.is_function) {
			if (field.name != "Sum")
				return;
			if constexpr (field.ValueTypeIsSameWith(overload_v<>(&Point::Sum)))
				cout << (p.*(field.value))() << endl;
			else if constexpr (field.ValueTypeIsSameWith(overload_v<float>(&Point::Sum)))
				cout << (p.*(field.value))(1.f) << endl;
			else
				assert(false);
		}
	});
}
