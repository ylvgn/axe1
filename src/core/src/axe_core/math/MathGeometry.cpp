#include "MathGeometry.h"

namespace axe { namespace Math {

template<class T>
bool Line2<T>::getClosestPoint(Vec2& outPoint, const Vec2& inPoint) const {
	Line3<T> line3(Vec3::s_xy0(start), Vec3::s_xy0(end));
	Vec3 closestPt;
	if (!line3.getClosestPoint(closestPt, Vec3::s_xy0(inPoint)))
		return false;

	outPoint = closestPt.xy();
	return true;
}

template<class T>
T Line2<T>::distanceToPoint(const Vec2& pt) const {
	Vec2 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return Math::inf<T>();
}

template<class T>
bool Line3<T>::getClosestPoint(Vec3& outPoint, const Vec3& inPoint) const {
	auto line = end - start;
	auto sqrLength = line.sqrLength();

	if (Math::equals0(sqrLength))
		return false;

	auto length = Math::sqrt(sqrLength);
	auto u = line / length; // normalized line
	auto v = inPoint - start;

	auto w = u.dot(v);
	w = Math::clamp(w, T(0), length);

	outPoint = start + u * w;
	return true;
}

template<class T>
T Line3<T>::distanceToPoint(const Vec3& pt) const {
	Vec3 closestPt;
	if (getClosestPoint(closestPt, pt)) {
		return closestPt.distance(pt);
	}
	return Math::inf<T>();
}


// explicit specialization to force VisualC check syntax in function body
#define AXE_MACRO_OP(T) \
	template class Line2<T>;     \
	template class Line3<T>;     \
	template class Plane3<T>;    \
	template class Sphere3<T>;   \
	template class Cylinder3<T>; \
	template class Capsule3<T>;  \
//----
	AXE_MACRO_OP(float)
	AXE_MACRO_OP(double)
#undef AXE_MACRO_OP

}} // namespace axe/Math
