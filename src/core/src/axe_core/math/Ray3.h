#pragma once

#include "Rect2.h"
#include "MathGeometry.h"

namespace axe {

template<class T>
class Ray3 {
	using This = typename Ray3<T>;
public:
	using Vec2		= Vec2<T>;
	using Vec3		= Vec3<T>;
	using Vec4		= Vec4<T>;
	using Mat4		= Mat4<T>;
	using Rect2		= Rect2<T>;
	using Line3		= Math::Line3<T>;

	struct HitTestResult
	{
		T	 distance = Math::inf<T>();
		Vec3 point;
		Vec3 normal;

		bool hasResult() const { return distance != Math::inf<T>(); }
	}; // HitTestResult

	constexpr explicit	Ray3() = default;
	constexpr			Ray3(const Vec3& origin_, const Vec3& dir_) noexcept : origin(origin_), dir(dir_) {}

	constexpr static Ray3 s_zero() { return Ray3(Vec3::s_zero(), Vec3::s_zero()); }

	static Ray3 s_unprojectFromInvMatrix(const Vec2& screenPos, const Mat4& invMat, const Rect2& viewport);

	bool getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const;
	bool getClosestPoint(Vec3& outPoint, const Line3& line, T minOnLine = 0, T maxOnLine = 1) const;

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Ray3(origin={}, dir={})", origin, dir);
	}

	Vec3 origin;
	Vec3 dir;
}; // Ray3

using Ray3f = Ray3<float>;
using Ray3d = Ray3<double>;

AXE_FORMATTER_T(class T, Ray3<T>)

} // namespace axe