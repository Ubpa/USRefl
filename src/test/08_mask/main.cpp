#include <USRefl/USRefl.h>

#include <iostream>
#include <array>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
	float z;
};

template<>
struct TypeInfo<Point> : TypeInfoBase<Point> {
	static constexpr FieldList fields = {
		Field{"x", &Point::x, AttrList{ Attr{ "not_serialize" } }},
		Field{"y", &Point::y, AttrList{ Attr{ "info", "hello" } }},
		Field{"z", &Point::z}
	};

	static constexpr AttrList attrs = {
		Attr{ "size", 8 }
	};
};

template<typename T, size_t... Ns>
constexpr auto GetXZ(std::index_sequence<Ns...>) {
	// get fields with name "x" or "z"
	constexpr auto masks = TypeInfo<T>::fields.Accumulate(
		std::array<bool, TypeInfo<T>::fields.size>{},
		[&, idx = 0](auto acc, auto field) mutable {
			acc[idx] = field.name == "x" || field.name == "z";
			idx++;
			return acc;
		}
	);
	constexpr auto fields = TypeInfo<T>::fields.Accumulate<masks[Ns]...>(
		ElemList<>{},
		[&](auto acc, auto field) {
			return acc.Push(field);
		}
	);
	return fields;
}
template<typename T>
constexpr auto GetXZ() {
	return GetXZ<T>(std::make_index_sequence<TypeInfo<T>::fields.size>{});
}

int main() {
	// get fields with name "x" or "z"
	constexpr auto fields = GetXZ<Point>();
	fields.ForEach([](auto field) {
		cout << field.name << endl;
	});
}
