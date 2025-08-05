#pragma once

#include "Quat4.h"
#include "Mat4.h"

namespace axe { namespace Math {

#if 0
#pragma mark ========= Sphere3 ============
#endif
template<class T>
class Sphere3 {
	using This = typename Sphere3<T>;
public:
	using Vec3 = Vec3<T>;

	constexpr explicit	Sphere3() = default;
	constexpr			Sphere3(const Vec3& center_, T radius_) noexcept : center(center_) , radius(radius_) {}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Sphere3(c={}, r={})", center, radius);
	}

	Vec3	center;
	T		radius;
}; // Sphere3


#if 0
#pragma mark ========= Triangle3 ============
#endif
template<class T>
class Triangle3 {
	using This = typename Triangle3<T>;
public:
	using Vec3 = Vec3<T>;

	constexpr explicit	Triangle3() = default;
	constexpr			Triangle3(const Vec3& v0_, const Vec3& v1_, const Vec3& v2_) noexcept : v0(v0_), v1(v1_), v2(v2_) {}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Triangle3(v0={}, v1={}, v2={})", v0, v1, v2);
	}

	Vec3 v0, v1, v2;
}; // Triangle3


#if 0
#pragma mark ========= Plane3 ============
#endif
template<class T>
class Plane3 {
	using This = typename Plane3<T>;
public:
	using Vec3 = Vec3<T>;

	constexpr explicit	Plane3() = default;
	constexpr			Plane3(const Vec3& normal_, T distance_)		noexcept : normal(normal_), distance(distance_) {}
	constexpr			Plane3(const Vec3& normal_, const Vec3& point_) noexcept : normal(normal_) , distance(normal_.dot(point_)) {}
						Plane3(const Triangle3<T>& tri)					noexcept { setByTriangle(tri); }

	void set(const Vec3& normal_, T distance_) noexcept {
		normal = normal_;
		distance = distance_;
	}

	void setByTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2) noexcept {
		normal = (v1 - v0).cross(v2 - v0).normal();
		distance = normal.dot(v0);
	}

	void setByTriangle(const Triangle3<T>& tri) noexcept {
		setByTriangle(tri.v0, tri.v1, tri.v2);
	}

	T dot(const Vec3& point) const noexcept { return normal.dot(point) - distance; }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Plane3(nl={}, distance={})", normal, distance);
	}

	Vec3	normal;
	T		distance; // distance from origin
}; // Plane3


#if 0
#pragma mark ========= Cylinder3 ============
#endif
template<class T>
class Cylinder3 {
	using This = typename Cylinder3<T>;
public:
	using Vec3 = Vec3<T>;

	constexpr explicit	Cylinder3() = default;
	constexpr			Cylinder3(const Vec3& start_, const Vec3& end_, const T& radius_) noexcept : start(start_), end(end_), radius(radius_) {}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Cylinder3(st={}, ed={}, r={})", start, end, radius);
	}

	Vec3 start;
	Vec3 end;
	T	 radius;
}; // Cylinder3


#if 0
#pragma mark ========= Capsule3 ============
#endif
template<class T>
class Capsule3 {
	using This = typename Capsule3<T>;
public:
	using Vec3 = Vec3<T>;

	constexpr explicit	Capsule3() = default;
	constexpr			Capsule3(const Vec3& start_, const Vec3& end_, const T& radius_) noexcept : start(start_), end(end_), radius(radius_) {}

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Capsule3(st={}, ed={}, r={})", start, end, radius);
	}

	Vec3 start;
	Vec3 end;
	T	 radius;
}; // Capsule3


#if 0
#pragma mark ========= Line2 ============
#endif
template<class T>
class Line2 {
	using This = typename Line2<T>;
public:
	using Vec2 = Vec2<T>;
	using Vec3 = Vec3<T>;

	constexpr explicit	Line2() = default;
	constexpr			Line2(const Vec2& start_, const Vec2& end_) noexcept : start(start_), end(end_) {}

	bool getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const;
	T	 distanceToPoint(const Vec2& pt) const;

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Line2(st={}, ed={})", start, end);
	}

	Vec2 start;
	Vec2 end;
}; // Line2


#if 0
#pragma mark ========= Line3 ============
#endif
template<class T>
class Line3 {
	using This = typename Line2<T>;
public:
	using Vec3 = Vec3<T>;

	constexpr explicit	Line3() = default;
	constexpr			Line3(const Vec3& start_, const Vec3& end_) noexcept : start(start_), end(end_) {}

	bool getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const;
	T	 distanceToPoint(const Vec3& pt) const;

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Line3(st={}, ed={})", start, end);
	}

	Vec3 start;
	Vec3 end;
}; // Line3


//----
using Sphere3f		= Sphere3<float>;
using Sphere3d		= Sphere3<double>;

using Plane3f		= Plane3<float>;
using Plane3d		= Plane3<double>;

using Triangle3f	= Triangle3<float>;
using Triangle3d	= Triangle3<double>;

using Capsule3f		= Capsule3<float>;
using Capsule3d		= Capsule3<double>;

using Line2f		= Line2<float>;
using Line2d		= Line2<double>;

using Line3f		= Line3<float>;
using Line3d		= Line3<double>;
}} // namespace axe/Math


namespace axe {

AXE_FORMATTER(Math::Sphere3f)
AXE_FORMATTER(Math::Sphere3d)

AXE_FORMATTER(Math::Plane3f)
AXE_FORMATTER(Math::Plane3d)

AXE_FORMATTER(Math::Triangle3f)
AXE_FORMATTER(Math::Triangle3d)

AXE_FORMATTER(Math::Capsule3f)
AXE_FORMATTER(Math::Capsule3d)

AXE_FORMATTER(Math::Line2f)
AXE_FORMATTER(Math::Line2d)

AXE_FORMATTER(Math::Line3f)
AXE_FORMATTER(Math::Line3d)

} // namespace axe