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
	static size_t num;

	// non-static member function
	float Sum() const {
		return x + y;
	}
	
	// overload function, default value
	float Sum(float z = 1.f) const {
		return x + y + z;
	}

	// static member function
	static T Dot(const Vec& lhs, const Vec& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
};