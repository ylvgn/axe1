#pragma once

#if AXE_CPLUSPLUS_17
	#define AXE_FALLTHROUGH		[[fallthrough]]
	#define AXE_NODISCARD		[[nodiscard]]
	#define AXE_IF_CONSTEXPR	if constexpr
#else
	#define AXE_FALLTHROUGH
	#define AXE_NODISCARD
	#define AXE_IF_CONSTEXPR	if
#endif

#define AXE_COMMA ,
#define AXE_EMPTY
#define AXE_ARGS(...) __VA_ARGS__
#define AXE_STRINGIFY(...)	#__VA_ARGS__

#define AXE_IDENTITY(x) x
#define AXE_CALL(M, ARGS) AXE_IDENTITY( M(ARGS) )

#define AXE_VA_ARGS_COUNT(...) AXE_IDENTITY( AXE_VA_ARGS_COUNT_INTERNAL(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1) )
#define AXE_VA_ARGS_COUNT_INTERNAL(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, COUNT, ...) COUNT

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

#define AXE_DUMP_VAR_SELECT(COUNT) AXE_DUMP_VAR_##COUNT
#define AXE_DUMP_VAR(...) AXE_IDENTITY(AXE_CALL(AXE_DUMP_VAR_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__) (__VA_ARGS__)))

#define AXE_DUMP_HEX(v) \
	do{ \
		String tmp; \
		StringUtil::binToHex(tmp, v); \
		AXE_LOG("DUMP_HEX: {}\n{}", #v, tmp); \
	} while(false) \
//----

#define	AXE_MOVE		::std::move
#define AXE_FORWARD(a)	::std::forward< decltype(a) >(a)
#define AXE_ASSERT(...)	assert(__VA_ARGS__)

#define AXE_FUNC		::std::function

#define AXE_STATIC_ASSERT_MSG	static_assert
#define AXE_STATIC_ASSERT(...)	AXE_STATIC_ASSERT_MSG(__VA_ARGS__, #__VA_ARGS__)

#define AXE_LOC	SrcLoc(__FILE__, __LINE__, AXE_FUNC_NAME_SZ)

#define AXE_ENUM_BITWISE_OPERATOR(T) \
	constexpr T operator~ (T  a)			{ return static_cast<T>(~enumInt(a)); } \
	constexpr T operator| (T  a, T b)		{ return static_cast<T>(enumInt(a) | enumInt(b)); } \
	constexpr T operator& (T  a, T b)		{ return static_cast<T>(enumInt(a) & enumInt(b)); } \
	constexpr T operator^ (T  a, T b)		{ return static_cast<T>(enumInt(a) ^ enumInt(b)); } \
	constexpr T operator>> (T a, T b)		{ return static_cast<T>(enumInt(a) >> enumInt(b)); } \
	constexpr T operator<< (T a, T b)		{ return static_cast<T>(enumInt(a) << enumInt(b)); } \
	constexpr void operator|=(T& a, T b)	{ a = static_cast<T>(enumInt(a) | enumInt(b)); } \
	constexpr void operator&=(T& a, T b)	{ a = static_cast<T>(enumInt(a) & enumInt(b)); } \
	constexpr void operator^=(T& a, T b)	{ a = static_cast<T>(enumInt(a) ^ enumInt(b)); } \
	constexpr void operator>>=(T& a, T b)	{ a = static_cast<T>(enumInt(a) >> enumInt(b)); } \
	constexpr void operator<<=(T& a, T b)	{ a = static_cast<T>(enumInt(a) << enumInt(b)); } \
//----

#define AXE_ENUM_ARITHMETIC_OPERATOR(T) \
	constexpr T operator+ (T  a, T b)		{ return static_cast<T>(enumInt(a) + enumInt(b)); } \
	constexpr T operator- (T  a, T b)		{ return static_cast<T>(enumInt(a) - enumInt(b)); } \
	constexpr T operator* (T  a, T b)		{ return static_cast<T>(enumInt(a) * enumInt(b)); } \
	constexpr T operator/ (T  a, T b)		{ return static_cast<T>(enumInt(a) / enumInt(b)); } \
	constexpr void operator+=(T& a, T b)	{ a = static_cast<T>(enumInt(a) + enumInt(b)); } \
	constexpr void operator-=(T& a, T b)	{ a = static_cast<T>(enumInt(a) - enumInt(b)); } \
	constexpr void operator*=(T& a, T b)	{ a = static_cast<T>(enumInt(a) * enumInt(b)); } \
	constexpr void operator/=(T& a, T b)	{ a = static_cast<T>(enumInt(a) / enumInt(b)); } \
//----

#define AXE_ENUM_ARITHMETIC_OPERATOR_INT(T) \
	constexpr T operator+ (T  a, int b)		{ return static_cast<T>(enumInt(a) + b); } \
	constexpr T operator- (T  a, int b)		{ return static_cast<T>(enumInt(a) - b); } \
	constexpr T operator* (T  a, int b)		{ return static_cast<T>(enumInt(a) * b); } \
	constexpr T operator/ (T  a, int b)		{ return static_cast<T>(enumInt(a) / b); } \
	constexpr T operator>> (T a, int b)		{ return static_cast<T>(enumInt(a) >> b); } \
	constexpr T operator<< (T a, int b)		{ return static_cast<T>(enumInt(a) << b); } \
	constexpr void operator+=(T& a, int b)	{ a = static_cast<T>(enumInt(a) + b); } \
	constexpr void operator-=(T& a, int b)	{ a = static_cast<T>(enumInt(a) - b); } \
	constexpr void operator*=(T& a, int b)	{ a = static_cast<T>(enumInt(a) * b); } \
	constexpr void operator/=(T& a, int b)	{ a = static_cast<T>(enumInt(a) / b); } \
	constexpr void operator>>=(T& a, int b) { a = static_cast<T>(enumInt(a) >> b); } \
	constexpr void operator<<=(T& a, int b) { a = static_cast<T>(enumInt(a) << b); } \
//----

#define AXE_ENUM_ALL_OPERATOR(T) \
	AXE_ENUM_BITWISE_OPERATOR(T) \
	AXE_ENUM_ARITHMETIC_OPERATOR(T) \
	AXE_ENUM_ARITHMETIC_OPERATOR_INT(T) \
//----

#define AXE_ENUM_STR__CASE(V, ...) case AXE_T::V: return #V;

#define AXE_ENUM_STR(T) \
	inline const char* enumStr(const T& v) { \
		using AXE_T = T; \
		switch (v) { \
			T##_ENUM_LIST(AXE_ENUM_STR__CASE) \
			default: AXE_ASSERT(false); return ""; \
		} \
	} \
//----

#define AXE_ENUM_TRY_PARSE__CASE(V, ...) if (str == #V) { outValue = AXE_T::V; return true; }

#define AXE_ENUM_TRY_PARSE(T) \
	inline bool enumTryParse(T& outValue, StrView str) { \
		using AXE_T = T; \
		T##_ENUM_LIST(AXE_ENUM_TRY_PARSE__CASE) \
		return false; \
	} \
//----

#define AXE_ENUM_STR_UTIL(T) \
	AXE_ENUM_STR(T) \
	AXE_ENUM_TRY_PARSE(T) \
//----

#define AXE_ENUM_DECLARE__ITEM(ITEM, VALUE) ITEM VALUE,

#define AXE_ENUM_DECLARE(T, BASE_TYPE) \
	enum class T : BASE_TYPE { \
		T##_ENUM_LIST(AXE_ENUM_DECLARE__ITEM) \
	}; \
//----

#define AXE_ENUM_CLASS(T, BASE_TYPE) \
	AXE_ENUM_DECLARE(T, BASE_TYPE) \
	AXE_ENUM_STR_UTIL(T) \
//----

#define AXE_NAMED_IO(SE, V)	SE.named_io(#V, V)

#define AXE_UNUSED(v) (void)(v);

#define AXE_STATIC_ASSERT_NO_MEMBER_CLASS(T) \
	class T##_Dummy : public T { uint8_t T##EnsureNoMemberIn; }; \
	AXE_STATIC_ASSERT(sizeof(T##_Dummy) == 1) \
//----