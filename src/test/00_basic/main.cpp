#include <USRefl.h>

#include <iostream>

using namespace Ubpa::USRefl;
using namespace std;

struct [[size("8")]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
};

template<>
struct TypeDecl<Point> {
	static constexpr std::tuple fields{
		GenField("x", &Point::x, std::array{ Meta{ "not_serialize" } }),
		GenField("y", &Point::y, std::array{ Meta{ "info", "hello" } }) };

	static constexpr std::array<Meta, 1> metas{ {
		{ "size", "8" }
	} };
};

int main() {
	const Point p{ 1,2 };

	Type<Point>::ForEachMeta([](const Meta& meta) {
		cout << "key   : " << meta.key << endl;
		cout << "value : " << meta.value << endl;
	});

	Type<Point>::ForEachField([](auto&& field) {
		cout << field.name << endl;
		field.ForEachMeta([](const Meta& meta) {
			cout << "key   : " << meta.key << endl;
			cout << "value : " << meta.value << endl;
		});
	});

	ForEachFieldOf(p, [](auto&& field) {
		cout << field << endl;
	});
}
