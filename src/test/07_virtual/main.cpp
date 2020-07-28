#include <USRefl/USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

struct A { float a; };
struct B : virtual A { float b; };
struct C : virtual A { float c; };
struct D : B, C { float d; };

template<>
struct Ubpa::USRefl::TypeInfo<A>
	: Ubpa::USRefl::TypeInfoBase<A>
{
	static constexpr AttrList attrs = {};

	static constexpr FieldList fields = {
		Field{"a", &A::a},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<B>
	: Ubpa::USRefl::TypeInfoBase<B, Base<A>>
{
	static constexpr AttrList attrs = {};

	static constexpr FieldList fields = {
		Field{"b", &B::b},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<C>
	: Ubpa::USRefl::TypeInfoBase<C, Base<A>>
{
	static constexpr AttrList attrs = {};

	static constexpr FieldList fields = {
		Field{"c", &C::c},
	};
};

template<>
struct Ubpa::USRefl::TypeInfo<D>
	: Ubpa::USRefl::TypeInfoBase<D, Base<B>, Base<C>>
{
	static constexpr AttrList attrs = {};

	static constexpr FieldList fields = {
		Field{"d", &D::d},
	};
};

int main() {
	cout << "// not fully support in MSVC++ 19.26 because of a bug (2020/07/17)" << endl;
	cout << "// https://developercommunity.visualstudio.com/content/problem/1116835/member-pointer-of-a-class-with-a-virtual-base-1.html" << endl;

	cout << "[Virtual Bases]" << endl;
	constexpr auto vbs = TypeInfo<D>::VirtualBases();
	vbs.ForEach([](auto info) {
		cout << info.name << endl;
	});

	cout << "[Tree]" << endl;
	TypeInfo<D>::DFS_ForEach([](auto t, size_t depth) {
		for (size_t i = 0; i < depth; i++)
			cout << "  ";
		cout << t.name << endl;
	});

	cout << "[field]" << endl;
	TypeInfo<D>::DFS_ForEach([](auto t, size_t) {
		t.fields.ForEach([](auto field) {
			cout << field.name << endl;
			});
	});

	cout << "[var]" << endl;
	D d;
	d.a = 1;
	d.b = 2;
	d.c = 3;
	d.d = 4;
	cout << "[var : left]" << endl;
	TypeInfo<D>::ForEachVarOf(std::move(d), [](auto&& var) {
		static_assert(std::is_rvalue_reference_v<decltype(var)>);
		cout << var << endl;
	});
	cout << "[var : right]" << endl;
	TypeInfo<D>::ForEachVarOf(d, [cnt = 0](auto&& var) mutable {
		static_assert(std::is_lvalue_reference_v<decltype(var)>);
		cout << cnt << ": " << var << endl;
		cnt++;
	});
}
