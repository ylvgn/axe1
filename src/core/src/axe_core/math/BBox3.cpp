#include "BBox3.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#else
	template BBox3<float>;
	template BBox3<double>;
#endif

} // namespace axe