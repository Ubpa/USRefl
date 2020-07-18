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
struct TypeInfo<A> : TypeInfoBase<A> {
	static constexpr FieldList fields = FieldList{
		Field{"a", &A::a }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<B> : TypeInfoBase<B, Base<A, true>> {
	static constexpr FieldList fields = FieldList{
		Field{"b", &B::b }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<C> : TypeInfoBase<C, Base<A, true>> {
	static constexpr FieldList fields = FieldList{
		Field{"c", &C::c }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct Ubpa::USRefl::TypeInfo<D> : TypeInfoBase<D, Base<B>, Base<C>> {
	static constexpr FieldList fields = FieldList{
		Field{"d", &D::d }
	};

	static constexpr AttrList attrs = {};
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
	TypeInfo<D>::DFS([](auto t, size_t depth) {
		for (size_t i = 0; i < depth; i++)
			cout << "  ";
		cout << t.name << endl;
	});

	cout << "[field]" << endl;
	TypeInfo<D>::DFS([](auto t, size_t) {
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
