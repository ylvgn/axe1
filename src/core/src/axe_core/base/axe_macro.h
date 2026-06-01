#pragma once

#define AXE_COMMA ,
#define AXE_EMPTY
#define AXE_ARGS(...)		__VA_ARGS__
#define AXE_STRINGIFY(...)	#__VA_ARGS__

#define AXE_IDENTITY(X) X
#define AXE_CALL(M, ARGS) AXE_IDENTITY( M(ARGS) )
// work around for MSVC bug - MSVC expands all __VA_ARGS__ as one argument (preserving commas)
// TODO: #define axe_CALL_MACRO(MACRO_NAME, ARGS) MACRO_NAME ARGS

#if AXE_CPLUSPLUS_20
	#define AXE_SIZEOF(...)	 ::axe::axe_consteval_Int(sizeof(__VA_ARGS__))
	#define AXE_ALIGNOF(...) ::axe::axe_consteval_Int(alignof(__VA_ARGS__))
#else
	#define AXE_SIZEOF(...)	 ::axe::axe_constexpr_Int(sizeof(__VA_ARGS__))
	#define AXE_ALIGNOF(...) ::axe::axe_constexpr_Int(alignof(__VA_ARGS__))
#endif

#define AXE_UNUSED(v) (void)(v)

#define AXE_FILE StrView(__FILE__)
#define AXE_LINE static_cast<int>(__LINE__)
#define AXE_LOC	 SrcLoc(AXE_FUNC_NAME_SZ, __FILE__, __LINE__)

#if AXE_COMPILER_CLANG
	#define AXE_LIFETIME_BOUND	[[clang::lifetimebound]]
#else
	#define AXE_LIFETIME_BOUND
#endif

#if AXE_COMPILER_GCC | AXE_COMPILER_CLANG
	#define AXE_GCC_PRAGMA(x)						_Pragma(#x)
	#define AXE_GCC_WARNING_PUSH()					AXE_GCC_PRAGMA(GCC diagnostic push)
	#define AXE_GCC_WARNING_POP()					AXE_GCC_PRAGMA(GCC diagnostic pop)
	#define AXE_GCC_WARNING_DISABLE(warning_name)	AXE_GCC_PRAGMA(GCC diagnostic ignored warning_name)
	#define AXE_GCC_WARNING_PUSH_AND_DISABLE(warning_name) \
		AXE_GCC_WARNING_PUSH() \
		AXE_GCC_WARNING_DISABLE(warning_name) \
	//------
#else
	#define AXE_GCC_PRAGMA(x)
	#define AXE_GCC_WARNING_PUSH()
	#define AXE_GCC_WARNING_POP()
	#define AXE_GCC_WARNING_DISABLE(warning_name)
	#define AXE_GCC_WARNING_PUSH_AND_DISABLE(warning_name)
#endif

#if AXE_COMPILER_VC
	#define AXE_VC_PRAGMA(x)						__pragma(x)
	#define AXE_VC_WARNING_PUSH()					__pragma(warning(push)) // #pragma warning(push)
	#define AXE_VC_WARNING_DISABLE(warning_code)	__pragma(warning(disable: warning_code)) // #pragma warning(disable : warning_code)
	#define AXE_VC_WARNING_POP()					__pragma(warning(pop))  // #pragma warning(pop)

	#define AXE_VC_WARNING_PUSH_AND_SET_LEVEL(LV)	__pragma(warning(push, LV)) 
	#define AXE_VC_WARNING_PUSH_AND_DISABLE(warning_code) \
		AXE_VC_WARNING_PUSH() \
		AXE_VC_WARNING_DISABLE(warning_code) \

#else
	#define AXE_VC_PRAGMA(x)
	#define AXE_VC_WARNING_PUSH() 
	#define AXE_VC_WARNING_DISABLE(warning_code)
	#define AXE_VC_WARNING_POP()

	#define AXE_VC_WARNING_PUSH_AND_SET_LEVEL(LV)
	#define AXE_VC_WARNING_PUSH_AND_DISABLE(warning_code)

#endif

//! will always return >= 1, even AXE_VA_ARGS_COUNT(), because it means one single empty arguemnt
#define AXE_VA_ARGS_COUNT(...) AXE_IDENTITY( AXE_VA_ARGS_COUNT_INTERNAL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1) )
#define AXE_VA_ARGS_COUNT_INTERNAL(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, COUNT, ...) COUNT

//---- Log
#define AXE_LOG(...)		do{ ::axe::Log::s_get()->write(axe::Log::Level::Info,    __VA_ARGS__); } while(false)
#define AXE_LOG_WARN(...)	do{ ::axe::Log::s_get()->write(axe::Log::Level::Warning, __VA_ARGS__); } while(false)
#define AXE_LOG_ERROR(...)	do{ ::axe::Log::s_get()->write(axe::Log::Level::Error,   __VA_ARGS__); } while(false)
#define AXE_LOG_FLUSH()		do{ ::axe::Log::s_get()->flush(); } while(false)

#define AXE_LOG_FUNC_NAME() AXE_LOG("FUNC {}", AXE_FUNC_FULLNAME_SZ)

#define AXE_WARN_ONCE(...)	do{ AXE_RUN_ONCE(::axe::Log::s_get()->write(::axe::Log::Level::Warning, __VA_ARGS__)); } while(false)

#define AXE_TODO(...)                                         \
	AXE_RUN_ONCE(                                             \
		::axe::TempString tmp = "[TODO] ";                    \
		tmp.appendFormat(__VA_ARGS__);                        \
		tmp.appendFormat("\n  - [{}]\n", AXE_LOC);            \
		::axe::Log::s_get()->onWrite(::axe::Log::Level::Warning, tmp)); \
//----

#define AXE_DUMP_VAR_1(v0)				do{ AXE_LOG("DUMP_VAR: {}=[{}]",							#v0, (v0)); } while(false)
#define AXE_DUMP_VAR_2(v0, v1)			do{ AXE_LOG("DUMP_VAR: {}=[{}], {}=[{}]",					#v0, (v0), #v1, (v1)); } while(false)
#define AXE_DUMP_VAR_3(v0, v1, v2)		do{ AXE_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}]",			#v0, (v0), #v1, (v1), #v2, (v2)); } while(false)
#define AXE_DUMP_VAR_4(v0, v1, v2, v3)	do{ AXE_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}]",	#v0, (v0), #v1, (v1), #v2, (v2), #v3, (v3)); } while(false)

#define AXE_DUMP_VAR_5(v0, v1, v2, v3, v4) \
	do{ AXE_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]" \
		, #v0, (v0) \
		, #v1, (v1) \
		, #v2, (v2) \
		, #v3, (v3) \
		, #v4, (v4) \
	); } while(false) \
//----

#define AXE_DUMP_VAR_6(v0, v1, v2, v3, v4, v5) \
	do{ AXE_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]" \
		, #v0, (v0) \
		, #v1, (v1) \
		, #v2, (v2) \
		, #v3, (v3) \
		, #v4, (v4) \
		, #v5, (v5) \
	); } while(false) \
//----

#define AXE_DUMP_VAR_7(v0, v1, v2, v3, v4, v5, v6) \
	do{ AXE_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]" \
		, #v0, (v0) \
		, #v1, (v1) \
		, #v2, (v2) \
		, #v3, (v3) \
		, #v4, (v4) \
		, #v5, (v5) \
		, #v6, (v6) \
	); } while(false) \
//----

#define AXE_DUMP_VAR_8(v0, v1, v2, v3, v4, v5, v6, v7) \
	do{ AXE_LOG("DUMP_VAR: {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}], {}=[{}]" \
		, #v0, (v0) \
		, #v1, (v1) \
		, #v2, (v2) \
		, #v3, (v3) \
		, #v4, (v4) \
		, #v5, (v5) \
		, #v6, (v6) \
		, #v7, (v7) \
	); } while(false) \
//----

#define AXE_DUMP_VAR_SELECT(COUNT) AXE_DUMP_VAR_ ## COUNT
#define AXE_DUMP_VAR(...) AXE_IDENTITY(AXE_CALL(AXE_DUMP_VAR_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__) (__VA_ARGS__)))

#define AXE_DUMP_HEX(v)                       \
	do                                        \
	{                                         \
		::axe::String tmp;                    \
		::axe::StringUtil::binToHex(tmp, v);  \
		AXE_LOG("DUMP_HEX: {}\n{}", #v, tmp); \
	} while (false) \
//----

#define AXE_CONCAT_1(v0)				v0
#define AXE_CONCAT_2(v0, v1)			v0 ## v1
#define AXE_CONCAT_3(v0, v1, v2)		v0 ## v1 ## v2
#define AXE_CONCAT_4(v0, v1, v2, v3)	v0 ## v1 ## v2 ## v3

#define AXE_CONCAT_SELECT(COUNT) AXE_CONCAT_ ## COUNT
#define AXE_CONCAT(...)			 AXE_IDENTITY(AXE_CALL(AXE_CONCAT_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__)))
//----

#define AXE_UNIQUE_NAME(NAME) AXE_CONCAT(axeUNIQUE_NAME_##NAME, __LINE__)
#define AXE_NO_NAME			  AXE_UNIQUE_NAME(Unnamed)

// usage: please check 'test_AXE_RUN_ONCE'
#define AXE_RUN_ONCE(EXPR)               \
	do                                   \
	{                                    \
		static bool AXE_NO_NAME = false; \
		if (!AXE_NO_NAME)                \
		{                                \
			AXE_NO_NAME = true;          \
			{ EXPR; }                    \
		}                                \
	} while (false) \
//----

#define	AXE_MOVE		::std::move
#define AXE_FORWARD(a)	::std::forward< decltype(a) >(a)

#define AXE_FUNC ::std::function

#define AXE_STATIC_ASSERT_MSG	static_assert
#define AXE_STATIC_ASSERT(...)	AXE_STATIC_ASSERT_MSG(__VA_ARGS__, #__VA_ARGS__)

#define AXE_SIMPLE_ERROR(ERROR_TYPE) \
	class ERROR_TYPE : public Error { \
		public: \
		ERROR_TYPE(const SrcLoc& srcLoc) : Error(srcLoc) {} \
		ERROR_TYPE(StrView msg, const SrcLoc& srcLoc) : Error(msg, srcLoc) {} \
	}; \
	AXE_FORMATTER(ERROR_TYPE) \
//----

#define AXE_THROW_ERROR(...) throw Error(Fmt(__VA_ARGS__), AXE_LOC)
#define AXE_THROW			 throw Error_Undefined(AXE_LOC)

#define AXE_ASSERT_IMPL_SELECT(COUNT) AXE_ASSERT_IMPL_##COUNT
#define AXE_ASSERT_IMPL_1(expr)						      do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr);								} } while(false)
#define AXE_ASSERT_IMPL_2(expr, msg)				      do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr, msg);							} } while(false)
#define AXE_ASSERT_IMPL_3(expr, msg, title)			      do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr, msg, title);					} } while(false)
#define AXE_ASSERT_IMPL_4(expr, msg, title, hacking_expr) do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr, msg, title); { hacking_expr; } } } while(false)
#define AXE_ASSERT_IMPL(...)   AXE_IDENTITY(AXE_CALL(AXE_ASSERT_IMPL_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__)))

#define AXE_ASSERT_ONCE(...)		 AXE_RUN_ONCE(AXE_ASSERT_IMPL(__VA_ARGS__))
#define AXE_ASSERT(expr)			 AXE_ASSERT_ONCE(expr, "", "---- ASSERT ----")
#define AXE_FATAL_ASSERT(expr)		 AXE_ASSERT_ONCE(expr, "", "---- FATAL ASSERT ----", axe_force_crash())
#define AXE_ASSERT_NOT_IMPLEMENTED() AXE_ASSERT_ONCE(false, AXE_FUNC_FULLNAME_SZ, "AXE_ASSERT_NOT_IMPLEMENTED");

#define AXE_VALIDATE(expr) ::axe::Error::s_validate(__FUNCTION__, __FILE__, __LINE__, expr, #expr, "")

#define AXE_ENUM_BITWISE_OPERATOR(T) \
	constexpr T		operator~  (T  a)		{ return static_cast<T>(~enumInt(a)); }				 \
	constexpr T		operator|  (T  a, T b)	{ return static_cast<T>(enumInt(a) | enumInt(b)); }  \
	constexpr T		operator&  (T  a, T b)	{ return static_cast<T>(enumInt(a) & enumInt(b)); }  \
	constexpr T		operator^  (T  a, T b)	{ return static_cast<T>(enumInt(a) ^ enumInt(b)); }  \
	constexpr T		operator>> (T  a, T b)	{ return static_cast<T>(enumInt(a) >> enumInt(b)); } \
	constexpr T		operator<< (T  a, T b)	{ return static_cast<T>(enumInt(a) << enumInt(b)); } \
	constexpr void	operator|= (T& a, T b)	{ a = static_cast<T>(enumInt(a) | enumInt(b)); }	 \
	constexpr void	operator&= (T& a, T b)	{ a = static_cast<T>(enumInt(a) & enumInt(b)); }	 \
	constexpr void	operator^= (T& a, T b)	{ a = static_cast<T>(enumInt(a) ^ enumInt(b)); }	 \
	constexpr void	operator>>=(T& a, T b)	{ a = static_cast<T>(enumInt(a) >> enumInt(b)); }	 \
	constexpr void	operator<<=(T& a, T b)	{ a = static_cast<T>(enumInt(a) << enumInt(b)); }	 \
//----

#define AXE_ENUM_ARITHMETIC_OPERATOR(T) \
	constexpr T		operator+ (T  a, T b)	{ return static_cast<T>(enumInt(a) + enumInt(b)); } \
	constexpr T		operator- (T  a, T b)	{ return static_cast<T>(enumInt(a) - enumInt(b)); } \
	constexpr T		operator* (T  a, T b)	{ return static_cast<T>(enumInt(a) * enumInt(b)); } \
	constexpr T		operator/ (T  a, T b)	{ return static_cast<T>(enumInt(a) / enumInt(b)); } \
	constexpr void	operator+=(T& a, T b)	{ a = static_cast<T>(enumInt(a) + enumInt(b)); }	\
	constexpr void	operator-=(T& a, T b)	{ a = static_cast<T>(enumInt(a) - enumInt(b)); }	\
	constexpr void	operator*=(T& a, T b)	{ a = static_cast<T>(enumInt(a) * enumInt(b)); }	\
	constexpr void	operator/=(T& a, T b)	{ a = static_cast<T>(enumInt(a) / enumInt(b)); }	\
//----

#define AXE_ENUM_ARITHMETIC_OPERATOR_INT(T) \
	constexpr T		operator+  (T  a, int b) { return static_cast<T>(enumInt(a) + b); }  \
	constexpr T		operator-  (T  a, int b) { return static_cast<T>(enumInt(a) - b); }  \
	constexpr T		operator*  (T  a, int b) { return static_cast<T>(enumInt(a) * b); }  \
	constexpr T		operator/  (T  a, int b) { return static_cast<T>(enumInt(a) / b); }  \
	constexpr T		operator>> (T  a, int b) { return static_cast<T>(enumInt(a) >> b); } \
	constexpr T		operator<< (T  a, int b) { return static_cast<T>(enumInt(a) << b); } \
	constexpr void	operator+= (T& a, int b) { a = static_cast<T>(enumInt(a) + b); }	 \
	constexpr void	operator-= (T& a, int b) { a = static_cast<T>(enumInt(a) - b); }	 \
	constexpr void	operator*= (T& a, int b) { a = static_cast<T>(enumInt(a) * b); }	 \
	constexpr void	operator/= (T& a, int b) { a = static_cast<T>(enumInt(a) / b); }	 \
	constexpr void	operator>>=(T& a, int b) { a = static_cast<T>(enumInt(a) >> b); }	 \
	constexpr void	operator<<=(T& a, int b) { a = static_cast<T>(enumInt(a) << b); }	 \
//----

#define AXE_ENUM_ALL_OPERATOR(T) \
	AXE_ENUM_BITWISE_OPERATOR(T) \
	AXE_ENUM_ARITHMETIC_OPERATOR(T) \
	AXE_ENUM_ARITHMETIC_OPERATOR_INT(T) \
//----

#define AXE_ENUM_STR__CASE(AXE_V, ...) case AXE_T::AXE_V: return #AXE_V;
#define AXE_ENUM_STR(T)                                                    \
	inline const char* enumStr(const T& v)                                 \
	{                                                                      \
		using AXE_T = T;                                                   \
		switch (v)                                                         \
		{                                                                  \
			T##_ENUM_LIST(AXE_ENUM_STR__CASE) default : AXE_ASSERT(false); \
			return "";                                                     \
		}                                                                  \
	}                                                                      \
//----

#define AXE_ENUM_TRY_PARSE__CASE(V, ...) if (str == #V) { outValue = AXE_T::V; return true; }
#define AXE_ENUM_TRY_PARSE(T)                                 \
	inline bool enumTryParse(T& outValue, StrView str)        \
	{                                                         \
		using AXE_T = T;                                      \
		T##_ENUM_LIST(AXE_ENUM_TRY_PARSE__CASE) return false; \
	}                                                         \
//----

#define AXE_ENUM_STR_UTIL(T) \
	AXE_ENUM_STR(T) \
	AXE_ENUM_TRY_PARSE(T) \
//----

#define AXE_ENUM_DECLARE__ITEM(ITEM, VALUE) ITEM VALUE,

#define AXE_ENUM_DECLARE(T, BASE_TYPE)        \
	enum class T : BASE_TYPE                  \
	{                                         \
		T##_ENUM_LIST(AXE_ENUM_DECLARE__ITEM) \
	};                                        \
//----

#define AXE_ENUM_CLASS(T, BASE_TYPE) \
	AXE_ENUM_DECLARE(T, BASE_TYPE) \
	AXE_ENUM_STR_UTIL(T) \
//----

#define AXE_NAMED_IO(SE, V)	SE.named_io(#V, V)

#define AXE_STATIC_ASSERT_NO_MEMBER_CLASS(T)  \
	AXE_GCC_WARNING_PUSH_AND_DISABLE("-Wunused-private-field") \
	class T##_Dummy : public T                \
	{                                         \
		uint8_t T##EnsureNoMemberIn;          \
	};                                        \
	AXE_GCC_WARNING_POP() \
	AXE_STATIC_ASSERT(sizeof(T##_Dummy) == 1) \
//----