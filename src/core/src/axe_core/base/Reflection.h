#pragma once

#include "axe_base.h"
#include <axe_core/log/Log.h>

#define AXE_DOWNCAST_GET_INSTANCE() \
	AXE_INLINE static This* s_instance() { return static_cast<This*>(Base::s_instance()); }
//----


#define AXE_RTTI_CLASS_COMMON__NOBASE_IMPL(T) \
private:\
	using This = T; \
public: \
	static const TypeInfo* s_getType(); \
	inline virtual const TypeInfo* getType() const { return s_getType(); } \
//----
#define AXE_RTTI_CLASS_COMMON__BASE_IMPL(T, BASE) \
private:\
	using This = T; \
	using Base = BASE; \
public: \
	static const TypeInfo* s_getType(); \
	inline virtual const TypeInfo* getType() const override { return s_getType(); } \
//----
#define AXE_RTTI_CLASS_COMMON_SELECT(COUNT) AXE_RTTI_CLASS_COMMON_##COUNT
#define AXE_RTTI_CLASS_COMMON_1(T)			AXE_RTTI_CLASS_COMMON__NOBASE_IMPL(T)
#define AXE_RTTI_CLASS_COMMON_2(T, BASE)	AXE_RTTI_CLASS_COMMON__BASE_IMPL(T, BASE)
#define AXE_RTTI_CLASS_COMMON(...)			AXE_IDENTITY(AXE_CALL(AXE_RTTI_CLASS_COMMON_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__)))
//----


#define AXE_ABSTRACT_CLASS_TYPE__NOBASE_IMPL(T) \
	AXE_RTTI_CLASS_COMMON(T) \
	class TI_Base : public TypeInfoInitNoBase<T> { \
	public: \
		TI_Base() : TypeInfoInitNoBase<T>(#T) {} \
	}; \
private: \
//----
#define AXE_ABSTRACT_CLASS_TYPE__BASE_IMPL(T, BASE) \
	AXE_RTTI_CLASS_COMMON(T, BASE) \
	class TI_Base : public TypeInfoInit<T, BASE> { \
	public: \
		TI_Base() : TypeInfoInit<T, BASE>(#T, nullptr) {} \
	}; \
private: \
//----
#define AXE_ABSTRACT_CLASS_TYPE_SELECT(COUNT) AXE_ABSTRACT_CLASS_TYPE_##COUNT
#define AXE_ABSTRACT_CLASS_TYPE_1(T)		  AXE_ABSTRACT_CLASS_TYPE__NOBASE_IMPL(T)
#define AXE_ABSTRACT_CLASS_TYPE_2(T, BASE)	  AXE_ABSTRACT_CLASS_TYPE__BASE_IMPL(T, BASE)
#define AXE_ABSTRACT_CLASS_TYPE(...)		  AXE_IDENTITY(AXE_CALL(AXE_ABSTRACT_CLASS_TYPE_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__)))
//----


#define AXE_CLASS_TYPE__NOBASE_IMPL(T) \
	AXE_RTTI_CLASS_COMMON(T) \
	class TI_Base : public TypeInfoInitNoBase<T> { \
	public: \
		TI_Base() : TypeInfoInitNoBase<T>(#T) {} \
	}; \
private: \
//----
#define AXE_CLASS_TYPE__BASE_IMPL(T, BASE) \
	AXE_RTTI_CLASS_COMMON(T, BASE) \
	class TI_Base : public TypeInfoInit<T, BASE> { \
	public: \
		TI_Base() : TypeInfoInit<T, BASE>(#T, &TypeCreator<T>) {} \
	}; \
private: \
//----
#define AXE_CLASS_TYPE_SELECT(COUNT) AXE_CLASS_TYPE_##COUNT
#define AXE_CLASS_TYPE_1(T)			 AXE_CLASS_TYPE__NOBASE_IMPL(T)
#define AXE_CLASS_TYPE_2(T, BASE)	 AXE_CLASS_TYPE__BASE_IMPL(T, BASE)
#define AXE_CLASS_TYPE(...)			 AXE_IDENTITY(AXE_CALL(AXE_CLASS_TYPE_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__)))
//----

namespace axe {

class Object;
class TypeInfo;

template<class T> inline const TypeInfo* TypeOf()			{ return T::s_getType(); }
template<class T> inline const TypeInfo* TypeOf(const T& v) { return TypeOf<T>(); }

#if 0
#pragma mark ========= FieldInfo ============
#endif
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
}; // FieldInfo
AXE_FORMATTER(FieldInfo)


#if 0
#pragma mark ========= TypeInfo ============
#endif
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

	static const TypeInfo* s_getType();
	AXE_INLINE virtual const TypeInfo* getType() const { return s_getType(); }

	const char*		name = "";
	const TypeInfo* base = nullptr;
	const TypeInfo* elementType = nullptr;
	Creator			creator;
	size_t			dataSize = 0;
	bool			isContainer = false;

protected:
	Span<const FieldInfo> _fields;
}; // TypeInfo
AXE_FORMATTER(TypeInfo)


#if 0
#pragma mark ========= TypeInfoInitNoBase ============
#endif
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
}; // TypeInfoInitNoBase

template<>
class TypeInfoInitNoBase<void> : public TypeInfo {
public:
	TypeInfoInitNoBase(const char* name_) {
		name = name_;
		dataSize = 0;
	}
}; // TypeInfoInitNoBase<void>


#if 0
#pragma mark ========= TypeInfoInit ============
#endif
template<class T, class BASE>
class TypeInfoInit : public TypeInfoInitNoBase<T> {
public:
	TypeInfoInit(const char* name_, Creator creator_) : TypeInfoInitNoBase<T>(name_) {
		AXE_STATIC_ASSERT(is_base_of_v<BASE, T>);
		base = TypeOf<BASE>();
		this->creator = creator_;
	}
}; // TypeInfoInit


template <class T, class... ARGS> AXE_NODISCARD
inline static Object* TypeCreator(ARGS&&... args) {
	return new T(AXE_FORWARD(args)...);
}

template <class DST> inline
DST* axe_cast(Object* obj) {
	if (!obj) return nullptr;
	const auto* ti = TypeOf<DST>();
	if (!ti) return nullptr;
	if (!ti->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(obj);
};

#define AXE_TYPEOF_PRIMITIVE(T) \
	template<> const TypeInfo* TypeOf<T>();
//----
	AXE_TYPEOF_PRIMITIVE(void)

	AXE_TYPEOF_PRIMITIVE(bool)

	AXE_TYPEOF_PRIMITIVE(i8 )
	AXE_TYPEOF_PRIMITIVE(i16)
	AXE_TYPEOF_PRIMITIVE(i32)
	AXE_TYPEOF_PRIMITIVE(i64)

	AXE_TYPEOF_PRIMITIVE(u8 )
	AXE_TYPEOF_PRIMITIVE(u16)
	AXE_TYPEOF_PRIMITIVE(u32)
	AXE_TYPEOF_PRIMITIVE(u64)

	AXE_TYPEOF_PRIMITIVE(f32 )
	AXE_TYPEOF_PRIMITIVE(f64 )
	AXE_TYPEOF_PRIMITIVE(f128)

	AXE_TYPEOF_PRIMITIVE(Char8 )
	AXE_TYPEOF_PRIMITIVE(Char16)
	AXE_TYPEOF_PRIMITIVE(Char32)
	AXE_TYPEOF_PRIMITIVE(CharW )
//----

#define AXE_TYPEOF_PRIMITIVE_IMP(T, NAME) \
	template<> const TypeInfo* TypeOf<T>() { \
		static TypeInfoInitNoBase<T> ti(NAME); \
		return &ti; \
	} \
//----

} // namespace axe