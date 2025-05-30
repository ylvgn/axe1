#pragma once

#include "Reflection.h"

namespace axe {

class Object;
template<> const TypeInfo* TypeOf<Object>();

class Object : public RefCountBase {
public:
	virtual ~Object() = default;
	virtual const TypeInfo* getType() const { return TypeOf<Object>(); }
};

} // namespace axe