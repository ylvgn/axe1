#pragma once

#include "axe_base.h"
#include <axe_core/pointer/SPtr.h>
#include <axe_core/log/Log.h>

namespace axe {

template<class T> inline constexpr
StrViewA axe_metatype_get_class_name() {
	auto* sig = AXE_FUNC_SIG;
	auto src = StrView_c_str(sig);

#if AXE_COMPILER_VC
	auto pair = src.split("axe_metatype_get_class_name<");
	pair = pair.second.splitBack(">(void)");
	return pair.first;
#else
	auto pair = src.split("axe_metatype_get_class_name() [T = ");
	pair = pair.second.splitBack("]");
	return pair.first;
#endif
}

struct IMetaTypeInit : public NonCopyable {
	IMetaTypeInit() = delete;
	//static NameId s_name() { return NameId(); } // TODO NameId::s_make
	static const char* s_name() { return nullptr; }
	using OwnFields = Tuple<>;
	using OwnAttrs  = Tuple<>;
};

struct NoBaseClass {
	NoBaseClass() = delete;
};

template<class T, class ENABLE = void>
struct BaseClassOf_Handler_ {
	using Type = typename T::_TYPE_INFO_Base;
};
template<class T>
struct BaseClassOf_Handler_ <T, enable_if_t<is_fundamental_v<T>> > {
	using Type = NoBaseClass;
};
template<class T> using BaseClassOf = typename BaseClassOf_Handler_<T>::Type;


template<class T>
struct MetaTypeInit_Handler_ {
	using MetaTypeInit = typename T::MetaTypeInit;
};

template<>
struct MetaTypeInit_Handler_<NoBaseClass> {
	using MetaTypeInit = IMetaTypeInit;
};

struct MetaAttr {
	static StrView	name() { return "MetaAttr"; }
};
struct MetaFieldBase : public NonCopyable {
	MetaFieldBase() = delete;
	using OwnFields = Tuple<>;
};

class Object;
class TypeInfo;

// use for declare out of class
template<class T> struct MutRttiInit_;
template<class T> struct MutRttiInit_FromMetaType_;
template<class T> struct MetaTypeInit_Helper_;

template<> struct MutRttiInit_FromMetaType_<NoBaseClass> {}; 

template<class T>
struct FinalMetaTypeOf_Handler_ {
	//using NameId = const char*; // TODO NameId::s_make
	
	using ObjThis                    = T;
	using ObjBase                    = BaseClassOf<T>;
	using Base_Handler               = FinalMetaTypeOf_Handler_<ObjBase>;
	using Base_MetaTypeInit          = typename MetaTypeInit_Handler_<ObjBase>::MetaTypeInit;
	using Base_FinalMetaType         = typename Base_Handler::FinalMetaType;
	using Base_AllFields             = typename Base_FinalMetaType::AllFields;
	using Base_AllAttrs              = typename Base_FinalMetaType::AllAttrs;
	using Potential_MetaTypeInit     = typename MetaTypeInit_Handler_<ObjThis>::MetaTypeInit;
	static constexpr bool hasOwnInit = !is_same_v<Potential_MetaTypeInit, Base_MetaTypeInit>;

	struct Empty_MetaTypeInit : public MetaTypeInit_Helper_<ObjBase> {};

	using MetaTypeInit = conditional_t<hasOwnInit, Potential_MetaTypeInit, Empty_MetaTypeInit>;
	using OwnFields    = typename MetaTypeInit::OwnFields;
	using OwnAttrs     = typename MetaTypeInit::OwnAttrs;
	
	struct FinalMetaType : public MetaTypeInit {
		//---- Combine all ---
		using AllFields = typename Base_AllFields::template JoinType<OwnFields>;
		using AllAttrs  = typename Base_AllAttrs ::template JoinType<OwnAttrs >;
	};
};
template<>
struct FinalMetaTypeOf_Handler_<NoBaseClass> {
	struct FinalMetaType : IMetaTypeInit {
		using AllFields = Tuple<>;
		using AllAttrs  = Tuple<>;
	};
};

template<class T> using MetaTypeOf			= typename FinalMetaTypeOf_Handler_<T>::FinalMetaType;

template<class T>
struct MetaTypeInit_Helper_ : public MetaTypeOf< BaseClassOf<T> > {
	using ObjThis = T;
	using ObjBase = typename T::_TYPE_INFO_Base;
	static const char* s_name() { // TODO NameId::s_make
		static TempString s;
		if (s.empty()) {
			auto view = axe_metatype_get_class_name<T>();
			s.assign(view);
		}
		return s.c_str();
	}
	
	using OwnFields = Tuple<>;
	using OwnAttrs  = Tuple<>;
};

template<class RTTI_INIT>
struct RttiInit_Make : public RTTI_INIT {
	RttiInit_Make() {
		if (auto* p = this->base) { //auto* base = RTTI_INIT::base; // TypeInfo::base
			this->allFields.reserve(p->allFields.size() + this->ownFields.size());
			this->allFields.appendRange(p->allFields);
		}
		for (auto* field : this->ownFields) {
			this->allFields.emplace_back(field);
		}
	}
};

template<class T>
struct Rtti_Handler_ {
	static TypeInfo* s_rtti() {
		static_assert(!is_const_v<T>);
		static_assert(!is_reference_v<T>);
		static_assert(!is_pointer_v<T>);
		
		static RttiInit_Make<MutRttiInit_FromMetaType_<T>> s;
		return &s;
	}
};

template<> struct Rtti_Handler_<NoBaseClass> {
	static TypeInfo* s_rtti() { return nullptr; }
};

template<class T> TypeInfo* rttiOf() { return Rtti_Handler_<remove_cv_t<T>>::s_rtti(); } // template<class T> inline const TypeInfo* TypeOf() { return T::s_getType(); }
template<class T> inline const TypeInfo* rttiOf(const T& v) { return rttiOf<T>(); }

#if 0
#pragma mark ========= FieldInfo ============
#endif
class FieldInfo {
public:
	using Getter = const void* (*)(const void* obj);
	using Setter = void (*)(void* obj, const void* value);
#if 1 // simple for now
	FieldInfo() = default;
#else // TODO will remove later
	template<class OBJ, class FIELD>
	FieldInfo(	const char* name_,
				FIELD OBJ::* ptr_,
				const FIELD& (*getter_)(const OBJ& obj) = nullptr,
				void (*setter_)(OBJ& obj, const FIELD& field) = nullptr)
		: name(name_)
		, fieldType(rttiOf<FIELD>())
		, offset(memberOffset(ptr_))
		, getter(reinterpret_cast<Getter>(getter_))
		, setter(reinterpret_cast<Setter>(setter_))
	{}
#endif
		  void* getValuePtr(      void* obj) const { return reinterpret_cast<      u8*>(obj) + offset;}
	const void* getValuePtr(const void* obj) const { return reinterpret_cast<const u8*>(obj) + offset;}

	template<class T>
	const T& getValue(const void* obj) const {
		AXE_ASSERT(rttiOf<T>() == fieldType);
		if (getter) {
			AXE_TODO("Let get/set allow assign instead of ctor init");
			return *reinterpret_cast<const T*>(getter(obj));
		} else {
			return *reinterpret_cast<const T*>(getValuePtr(obj));
		}
	}

	template<class T>
	void setValue(void* obj, const T& value) const {
		AXE_ASSERT(rttiOf<T>() == fieldType);
		if (setter) {
			AXE_TODO("Let get/set allow assign instead of ctor init");
			setter(obj, &value);
		} else {
			*reinterpret_cast<T*>(getValuePtr(obj)) = value;
		}
	}

	void onFormat(fmt::format_context& ctx) const;

	const TypeInfo*	fieldOwner	= nullptr;
	const TypeInfo* fieldType	= nullptr;
	
	TempString name; //const char* name			= ""; TODO NameId::s_make
	intptr_t offset				= INT_MAX;
	
	Getter getter				= nullptr; // TODO may use new handle
	Setter setter				= nullptr; // TODO may use new handle
	
}; // FieldInfo
AXE_FORMATTER(FieldInfo)


#if 0
#pragma mark ========= TypeInfo ============
#endif
using Rtti = TypeInfo; // TODO rename
class TypeInfo {
public:
	using Creator = Object * (*)();

	using InNameId = const char*; // TODO NameId::s_make
	
	virtual ~TypeInfo() = default;

	Object* createObject() const {
		if (!creator) return nullptr;
		return creator();
	}

	bool isKindOf(const TypeInfo* target) const {
		const TypeInfo* p = this;
		while(p) {
			if (p == target) return true;
			p = p->base;
		}
		return false;
	}

	template<class DST> inline
	bool isKindOf() const {
		return isKindOf(rttiOf<DST>());
	}

	Span<const FieldInfo* const> fields() const { return ownFields; }

	void onFormat(fmt::format_context& ctx) const;

	TempString name; // const char*		name = ""; TODO NameId::s_make
	const TypeInfo* base = nullptr;

	const TypeInfo* elementType = nullptr;	// TODO container type
	Creator			creator;			 	// TODO container type
	size_t			dataSize = 0;		 	// TODO container type
	bool			isContainer = false; 	// TODO container type

	void addField(InNameId name_, Rtti* fieldType_, Int offset_) {
		AXE_ASSERT(ownFieldsDict.count(name_) == 0);
		auto& field		 = ownFieldsDict[name_];
		field.name		 = name_;
		field.fieldOwner = this;
		field.fieldType  = fieldType_;
		field.offset     = offset_;
		ownFields.emplace_back(&field);
	}

	template<class OBJ, class FIELD>
	void addField(InNameId name_, FIELD OBJ::*ptrToField) {
		addField(name_, rttiOf<FIELD>(), memberOffset(ptrToField));
	}

	Vector<const FieldInfo*>    allFields;
	Vector<const FieldInfo*>	ownFields;
	StringMap<FieldInfo>		ownFieldsDict;
}; // TypeInfo
AXE_FORMATTER(TypeInfo)

#if 0
#pragma mark ========= RttiObject ============
#endif
class RttiObject : public RefCountBase {
public:
	RttiObject() = default;
	virtual ~RttiObject() = default;
	
	AXE_META_TYPE(RttiObject, NoBaseClass) {};
	
	static  Rtti* s_rtti ()		{ return rttiOf<This>(); }
	virtual Rtti* rtti() const 	{ return rttiOf<This>(); }
};


#if 0
#pragma mark ========= MutRttiInit_FromMetaType_ ============
#endif
template<class T>
struct MutRttiInit_FromMetaType_ : public TypeInfo {
	using ObjThis      = T;
	using ObjBase      = BaseClassOf<T>;

	using MetaType     = MetaTypeOf<T>;
	using BaseMetaType = MetaTypeOf<ObjBase>;

	static constexpr bool noBase = is_same_v<ObjBase, NoBaseClass>;
	
	struct OwnField_Handler {
		template<Int Index, class Field>
		static void onEach(MutRttiInit_FromMetaType_* rtti) {
			rtti->addField(Field::s_name(), rttiOf<typename Field::FieldType>(), Field::s_offset());
		}
	};

	MutRttiInit_FromMetaType_() {
//		static_assert(Type_IsBaseOf<IMetaType, MetaType>, "MetaType must based on IMetaType");
		this->base = rttiOf<ObjBase>();
		auto view = axe_metatype_get_class_name<T>();	AXE_TODO("return value should NameId::s_make");
		this->name.assign(view);						AXE_TODO("template require ::axe::is_convertible then can easy use \"this->name = view;\"");
		using OwnFields = typename MetaType::OwnFields;
		this->ownFields.reserve(OwnFields::kSize);
		OwnFields::template ForEachType<OwnField_Handler>(this);
	}
};


template <class T, class... ARGS> AXE_NODISCARD
inline static Object* TypeCreator(ARGS&&... args) {
	return new T(AXE_FORWARD(args)...);
}

template <class DST> inline
DST* axe_cast(Object* obj) {
	if (!obj) return nullptr;
	const auto* ti = rttiOf<DST>();
	if (!ti) return nullptr;
	if (!ti->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(obj);
};

AXE_META_TYPE_INIT_SIMPLE(void)
AXE_META_TYPE_INIT_SIMPLE(bool)

AXE_META_TYPE_INIT_SIMPLE( i8)
AXE_META_TYPE_INIT_SIMPLE(i16)
AXE_META_TYPE_INIT_SIMPLE(i32)
AXE_META_TYPE_INIT_SIMPLE(i64)

AXE_META_TYPE_INIT_SIMPLE( u8)
AXE_META_TYPE_INIT_SIMPLE(u16)
AXE_META_TYPE_INIT_SIMPLE(u32)
AXE_META_TYPE_INIT_SIMPLE(u64)

AXE_META_TYPE_INIT_SIMPLE( f32)
AXE_META_TYPE_INIT_SIMPLE( f64)
AXE_META_TYPE_INIT_SIMPLE(f128)

} // namespace axe