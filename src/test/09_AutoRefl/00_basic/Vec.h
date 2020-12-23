#include <tuple>

namespace Ubpa::Nested {
	template<typename T> // template
	struct [[size(sizeof(T))]] /*compile-time attr*/ Vec {
		// default constructor
		Vec() : x{ 0.f }, y{ 0.f } {}
		// overload constructor
		Vec(T x, T y) : x{ x }, y{ y } {}
		// destructor
		~Vec() {}

		[[not_serialize]] // non-value attr
		T x;
		[[info("hello"), maximum(10.f)]] // attr list
		T y;
		// static member variable
		inline static std::size_t num{ 0 };

		// non-static member function
		float Sum() const {
			return x + y;
		}
		// overload function, default value
		float Sum(float z, float o = 1.f) const {
			return x + y + z + o;
		}

		// static member function
		static T Dot(const Vec& lhs, const Vec& rhs) {
			return lhs.x * rhs.x + lhs.y * rhs.y;
		}
	};
}

// CMake will generate this file before building
// it contains the reflection declaration of Vec
#include "Vec_AutoRefl.inl"
