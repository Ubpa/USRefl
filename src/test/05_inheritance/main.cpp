#include <USRefl/USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

struct A {
	float a;
};
struct B : A {
	float b;
};
struct C : A {
	float c;
};
struct D : B, C {
	float d;
};

template<>
struct TypeInfo<A> : TypeInfoBase<A> {
	static constexpr FieldList fields = FieldList{
		Field{"a", &A::a }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<B> : TypeInfoBase<B, Base<A>> {
	static constexpr FieldList fields = FieldList{
		Field{"b", &B::b }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<C> : TypeInfoBase<C, Base<A>> {
	static constexpr FieldList fields = FieldList{
		Field{"c", &C::c }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct TypeInfo<D> : TypeInfoBase<D, Base<B>, Base<C>> {
	static constexpr FieldList fields = FieldList{
		Field{"d", &D::d }
	};

	static constexpr AttrList attrs = {};
};

int main() {
	TypeInfo<D>::DFS([](auto t, size_t depth) {
		for (size_t i = 0; i < depth; i++)
			cout << "  ";
		cout << t.name << endl;
	});

	cout << "[non DFS]" << endl;
	TypeInfo<D>::fields.ForEach([](auto field){
		cout << field.name << endl;
	});

	cout << "[DFS]" << endl;
	TypeInfo<D>::DFS([](auto t, size_t) {
		t.fields.ForEach([](auto field) {
			cout << field.name << endl;
		});
	});

	cout << "[var]" << endl;
	D d;
	d.B::a = 1;
	d.C::a = 2;
	d.b = 3;
	d.c = 4;
	d.d = 5;
	cout << "[left]" << endl;
	TypeInfo<D>::ForEachVarOf(std::move(d), [cnt = 0](auto&& var) mutable {
		static_assert(std::is_rvalue_reference_v<decltype(var)>);
		cout << cnt << ": " << var << endl;
		cnt++;
	});
	cout << "[right]" << endl;
	TypeInfo<D>::ForEachVarOf(d, [cnt = 0](auto&& var) mutable {
		static_assert(std::is_lvalue_reference_v<decltype(var)>);
		cout << cnt << ": " << var << endl;
		cnt++;
	});
}
