#include <USRefl.h>

#include <iostream>

#include <cassert>

using namespace Ubpa::USRefl;
using namespace std;

struct A {
	float a;
};
struct B {
	float b;
};
struct C : A {
	float c;
};
struct D : B, C {
	float d;
};

template<>
struct Type<A> {
	static constexpr std::string_view name = "A";
	using type = A;

	static constexpr FieldList fields = {
		Field{"a", &A::a, AttrList{} }
	};

	static constexpr AttrList attrs = {};

	static constexpr TypeList subclasses = {};
};
template<>
struct Type<B> {
	static constexpr std::string_view name = "B";
	using type = B;

	static constexpr FieldList fields = {
		Field{"b", &B::b, AttrList{} }
	};

	static constexpr AttrList attrs = {};

	static constexpr TypeList subclasses = { };
};
template<>
struct Type<C> {
	static constexpr std::string_view name = "C";
	using type = C;

	static constexpr FieldList fields = {
		Field{"c", &C::c, AttrList{} }
	};

	static constexpr AttrList attrs = {};

	static constexpr TypeList subclasses = { Type<A>{} };
};
template<>
struct Type<D> {
	static constexpr std::string_view name = "D";
	using type = D;

	static constexpr FieldList fields = {
		Field{"d", &D::d, AttrList{} }
	};

	static constexpr AttrList attrs = {};

	static constexpr TypeList subclasses = { Type<B>{}, Type<C>{} };
};

template<typename T>
void dump() {
	cout << Type<T>::name << "(";
	Type<T>::subclasses.ForEach([](auto t) {
		dump<typename decltype(t)::type>();
	});
	cout << ")";
}

int main() {
	dump<D>(); // D(B()C(A()))
}
