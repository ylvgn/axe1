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
}; // Object

} // namespace axe