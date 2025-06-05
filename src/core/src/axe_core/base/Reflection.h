#pragma once

#include "axe_base.h"
#include <axe_core/log/Log.h>

#define AXE_STRUCT_TYPE(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
	class TI_Base : public TypeInfoInit<T, BASE> { \
	public: \
		TI_Base() : TypeInfoInit<T, BASE>(#T, nullptr) {} \
	}; \
public: \
	static const TypeInfo* s_getType(); \
	virtual const TypeInfo* getType() const override { return s_getType(); } \
//----

#define AXE_ABSTRACT_OBJECT_TYPE_BASE(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
public: \
	static const TypeInfo* s_getType(); \
	virtual const TypeInfo* getType() const override { return s_getType(); } \
//----

#define AXE_ABSTRACT_OBJECT_TYPE(T, BASE) \
	AXE_ABSTRACT_OBJECT_TYPE_BASE(T, BASE) \
	class TI_Base : public TypeInfoInit<T, BASE> { \
	public: \
		TI_Base() : TypeInfoInit<T, BASE>(#T, nullptr) {} \
	}; \
private: \
//----

#define AXE_OBJECT_TYPE(T, BASE) \
	AXE_ABSTRACT_OBJECT_TYPE_BASE(T, BASE) \
	class TI_Base : public TypeInfoInit<T, BASE> { \
	public: \
		TI_Base() : TypeInfoInit<T, BASE>(#T, &TypeCreator<T>) {} \
	}; \
private: \
//----


namespace axe {

class Object;
class TypeInfo;

template<class T> inline const TypeInfo* TypeOf()			{ return T::s_getType(); }
template<class T> inline const TypeInfo* TypeOf(const T& v) { return TypeOf<T>(); }

class FieldInfo {
public:
	using Getter = const void* (*)(const void* obj);
	using Setter = void (*)(void* obj, const void* value);

	template<class OBJ, class FIELD>
	FieldInfo(	const char* name_,
				FIELD OBJ::* ptr_,
				const FIELD& (*getter_)(const OBJ& obj) = nullptr,
				void (*setter_)(OBJ& obj, const FIELD& field) = nullptr)
		: name(name_)
		, fieldType(TypeOf<FIELD>())
		, offset(memberOffset(ptr_))
		, getter(reinterpret_cast<Getter>(getter_))
		, setter(reinterpret_cast<Setter>(setter_))
	{}

		  void* getValuePtr(      void* obj) const { return reinterpret_cast<      u8*>(obj) + offset; }
	const void* getValuePtr(const void* obj) const { return reinterpret_cast<const u8*>(obj) + offset; }

	template<class T>
	const T& getValue(const void* obj) const {
		AXE_ASSERT(TypeOf<T>() == fieldType);
		if (getter) {
			return *reinterpret_cast<const T*>(getter(obj));
		} else {
			return *reinterpret_cast<const T*>(getValuePtr(obj));
		}
	}

	template<class T>
	void setValue(void* obj, const T& value) const {
		AXE_ASSERT(TypeOf<T>() == fieldType);
		if (setter) {
			setter(obj, &value);
		} else {
			*reinterpret_cast<T*>(getValuePtr(obj)) = value;
		}
	}

	void onFormat(fmt::format_context& ctx) const;

	const char* name			= "";
	const TypeInfo* fieldType	= nullptr;
	intptr_t offset				= 0;
	Getter getter				= nullptr;
	Setter setter				= nullptr;
};
AXE_FORMATTER(FieldInfo)

class TypeInfo {
public:
	using Creator = Object * (*)();

	Object* createObject() const {
		if (!creator) return nullptr;
		return creator();
	}

	bool isKindOf(const TypeInfo* target) const {
		if (!target) return nullptr;
		const TypeInfo* p = this;
		while(p) {
			if (p == target) return true;
			p = p->base;
		}
		return false;
	}

	template<class DST> inline
	bool isKindOf() const {
		return isKindOf(TypeOf<DST>());
	};

	Span<const FieldInfo> fields() const { return _fields; }

	void onFormat(fmt::format_context& ctx) const;

	const char*		name = "";
	const TypeInfo* base = nullptr;
	const TypeInfo* elementType = nullptr;
	Creator			creator;
	size_t			dataSize = 0;
	bool			isContainer = false;

protected:
	Span<const FieldInfo> _fields;
};
AXE_FORMATTER(TypeInfo)

template<class T>
class TypeInfoInitNoBase : public TypeInfo {
public:
	TypeInfoInitNoBase(const char* name_) {
		name = name_;
		dataSize = sizeof(T);
	}

	template<size_t N>
	void setFields(const FieldInfo(&fi)[N]) {
		_fields = fi;
	}
};

template<class T> inline
static Object* TypeCreator() {
	return new T();
}

template<class T, class BASE>
class TypeInfoInit : public TypeInfoInitNoBase<T> {
public:
	TypeInfoInit(const char* name_, Creator creator_) : TypeInfoInitNoBase<T>(name_) {
		AXE_STATIC_ASSERT(TypeTraits::isBaseOf<BASE, T>::value);
		base = TypeOf<BASE>();
		this->creator = creator_;
	}
};

template<class DST> inline
DST* axe_cast(Object* obj) {
	if (!obj) return nullptr;
	const auto* ti = TypeOf<DST>();
	if (!ti) return nullptr;
	if (!ti->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(obj);
};

#define AXE_TYPEOF_SIMPLE(T) \
	template<> const TypeInfo* TypeOf<T>();
//----

#define AXE_TYPEOF_SIMPLE_IMP(T, NAME) \
	template<> const TypeInfo* TypeOf<T>() { \
		static TypeInfoInitNoBase<T> ti(NAME); \
		return &ti; \
	} \
//----

AXE_TYPEOF_SIMPLE(f32)
AXE_TYPEOF_SIMPLE(f64)
AXE_TYPEOF_SIMPLE(f128)

AXE_TYPEOF_SIMPLE(i8 )
AXE_TYPEOF_SIMPLE(i16)
AXE_TYPEOF_SIMPLE(i32)
AXE_TYPEOF_SIMPLE(i64)

AXE_TYPEOF_SIMPLE(u8)
AXE_TYPEOF_SIMPLE(u16)
AXE_TYPEOF_SIMPLE(u32)
AXE_TYPEOF_SIMPLE(u64)

AXE_TYPEOF_SIMPLE(Char8)
AXE_TYPEOF_SIMPLE(Char16)
AXE_TYPEOF_SIMPLE(Char32)
AXE_TYPEOF_SIMPLE(CharW)

} // namespace axe