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
	static constexpr TypeList subclasses = {};

	static constexpr FieldList fields = FieldList{
		Field{"a", &A::a, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct Type<B> {
	static constexpr std::string_view name = "B";
	using type = B;
	static constexpr TypeList subclasses = {};

	static constexpr FieldList fields = FieldList{
		Field{"b", &B::b, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct Type<C> {
	static constexpr std::string_view name = "C";
	using type = C;
	static constexpr TypeList subclasses = { Type<A>{} };

	static constexpr FieldList fields = FieldList{
		Field{"c", &C::c, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<>
struct Type<D> {
	static constexpr std::string_view name = "D";
	using type = D;
	static constexpr TypeList subclasses = { Type<B>{}, Type<C>{} };

	static constexpr FieldList fields = FieldList{
		Field{"d", &D::d, AttrList{} }
	};

	static constexpr AttrList attrs = {};
};

template<typename T>
void dump(size_t depth = 0) {
	for (size_t i = 0; i < depth; i++)
		cout << "  ";
	cout << Type<T>::name << endl;
	Type<T>::subclasses.ForEach([depth](auto t) {
		dump<typename decltype(t)::type>(depth + 1);
	});
}

int main() {
	dump<D>();

	// TODO: walk recursively
	Type<D>::fields.ForEach([](auto field){
		cout << field.name << endl;
	});
}
