#pragma once

#include "Reflection.h"
#include "../pointer/WPtr.h"

namespace axe {

class Object;
template<> const TypeInfo* TypeOf<Object>();

class Object : public RefCountBase {
public:
	virtual ~Object() = default;
	virtual const TypeInfo* getType() const { return TypeOf<Object>(); }

	void		setDebugName(StrView name)	{ _debugName.assign(name); }
	StrView		debugName	() const		{ return _debugName; }

private:
	TempString _debugName;
}; // Object

} // namespace axe