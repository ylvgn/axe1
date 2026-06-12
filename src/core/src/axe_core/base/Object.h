#pragma once

#include "Reflection.h"
#include "../pointer/WPtr.h"

namespace axe {

class Object : public RttiObject {
	AXE_RTTI_INFO(Object, RttiObject)
public:
	void		setName(StrView name_)	{ _name.assign(name_); }
	StrView		name	() const		{ return _name; }
protected:
	TempString _name;
}; // Object

} // namespace axe