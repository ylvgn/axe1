#pragma once

#include "Reflection.h"
#include "../pointer/WPtr.h"

namespace axe {

class Object : public RefCountBase, public TypeInfo {
	AXE_CLASS_TYPE(Object, TypeInfo)
public:
	virtual ~Object() = default;

	void		setDebugName(StrView debugName)	{ _debugName.assign(debugName); }
	StrView		debugName	() const			{ return _debugName; }

private:
	TempString _debugName;
}; // Object

} // namespace axe