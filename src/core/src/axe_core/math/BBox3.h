#pragma once

#include "Vec3.h"
#include "Mat4.h"

namespace axe {

template<class T>
class BBox3 {
	using This = typename BBox3<T>;
public:
	using Vec3 = Vec3<T>;
	using Mat4 = Mat4<T>;

	explicit constexpr BBox3() = default;
			 constexpr BBox3(const Vec3& min_, const Vec3& max_) noexcept : min(min_), max(max_) {}

	static BBox3 s_invalid() {
		return BBox3(Vec3(1,1,1), Vec3(-1,-1,-1));
	}

	bool isValid() const { return min.x <= max.x; }

	void reset() {
		min.set( 1, 1, 1);
		max.set(-1,-1,-1);
	}

	void encapsulate(const Vec3& pt);

	void getPoints(Vec3 outPoints[8], const Mat4& matrix) const;

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "BBox3(\n  min={},\n  max={})", min, max);
	}

	Vec3 min;
	Vec3 max;
}; // BBox3

using BBox3f = BBox3<float>;
using BBox3d = BBox3<double>;

AXE_FORMATTER_T(class T, BBox3<T>)


template<class T> inline
void BBox3<T>::getPoints(Vec3 outPoints[8], const Mat4& matrix) const {
	auto& a = min;
	auto& b = max;

	outPoints[0] = matrix.mulPoint4x3(Vec3(a.x, a.y, a.z));
	outPoints[1] = matrix.mulPoint4x3(Vec3(b.x, a.y, a.z));
	outPoints[2] = matrix.mulPoint4x3(Vec3(b.x, b.y, a.z));
	outPoints[3] = matrix.mulPoint4x3(Vec3(a.x, b.y, a.z));

	outPoints[4] = matrix.mulPoint4x3(Vec3(a.x, a.y, b.z));
	outPoints[5] = matrix.mulPoint4x3(Vec3(b.x, a.y, b.z));
	outPoints[6] = matrix.mulPoint4x3(Vec3(b.x, b.y, b.z));
	outPoints[7] = matrix.mulPoint4x3(Vec3(a.x, b.y, b.z));
}

template<class T> inline
void BBox3<T>::encapsulate(const Vec3& pt) {
	if (!isValid()) {
		min = pt;
		max = pt;
	} else {
		min = Math::min(min, pt);
		max = Math::max(max, pt);
	}
}

} // namespace axe