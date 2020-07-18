#include <USRefl/USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

template <typename... Args>
struct Overload {
	template <typename R, typename T>
	constexpr auto operator()(R(T::* func_ptr)(Args...)) const {
		return func_ptr;
	}
	template <typename R, typename T>
	constexpr auto operator()(R(T::* func_ptr)(Args...) const) const {
		return func_ptr;
	}
	template <typename R>
	constexpr auto operator()(R(*func_ptr)(Args...)) const {
		return func_ptr;
	}
};

template <typename... Args> constexpr Overload<Args...> overload_v{};

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
struct TypeInfo<Point> : TypeInfoBase<Point> {
	using type = Point;

	static constexpr FieldList fields = {
		Field{"x", &Point::x, AttrList{ Attr{ "not_serialize", true } }},
		Field{"y", &Point::y, AttrList{ Attr{ "info", "hello" } }},
		Field{"Sum", overload_v<>(&Point::Sum)},
		Field{"Sum", overload_v<float>(&Point::Sum)}
	};

	static constexpr AttrList attrs = {
		Attr{ "size", 8 }
	};
};

int main() {
	Point p{ 1,2 };

	TypeInfo<Point>::fields.ForEach([p](auto field) {
		if constexpr (field.is_func) {
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
