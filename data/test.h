struct [[size(8)]] Point {
	[[not_serialize]]
	float x;
	[[info("hello")]]
	float y;
	[[attr("great")]]
	float Sum0() const{
		return x + y;
	}
	[[attr("great too")]]
	float Sum1(float z) const;
};
