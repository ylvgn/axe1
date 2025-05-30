#include <axe_core/base/UnitTest.h>
#include <axe_core/string/Fmt.h>

namespace axe {

template<class MyString>
class Test_String : public UnitTestBase {
	using MyStrView = typename MyString::view_type;
	using MyChar = typename MyString::value_type;
public:	
	void test_compare(const char* sz_) {
		MyString tmp;
		UtfUtil::convert(tmp, sz_);

		const MyChar* sz = tmp.c_str();

		MyString str(sz);
		AXE_TEST_CHECK(str == str.view());
		AXE_TEST_CHECK(str == sz);

		MyStrView view(sz);
		AXE_TEST_CHECK(view == str);
		AXE_TEST_CHECK(view == sz);

		AXE_TEST_CHECK(str == view);
		AXE_TEST_CHECK(sz == view);
	}

	void test_compare(const wchar_t* sz_) {
		MyString tmp;
		UtfUtil::convert(tmp, sz_);
	
		const MyChar* sz = tmp.c_str();
	
		MyString str(sz);
	
		AXE_TEST_CHECK(str == str.view());
		AXE_TEST_CHECK(str == sz);
	
		MyStrView view(sz);
		AXE_TEST_CHECK(view == str);
		AXE_TEST_CHECK(view == sz);
	
		AXE_TEST_CHECK(str == view);
		AXE_TEST_CHECK(sz == view);
	}

	template<size_t N>
	void my_memcpy(wchar_t(&dst)[N], const MyString& src) {
		if (N <= 0) return;
		wmemcpy_s(dst, N, src.c_str(), src.size());
		dst[N - 1] = L'\0';
	}

	template<size_t N>
	void my_memcpy(char(&dst)[N], const MyString& src) {
		if (N <= 0) return;
		memcpy_s(dst, N, src.c_str(), src.size());
		dst[N - 1] = '\0';
	}

	template<size_t N>
	void test_compare(const char (&sz_)[N]) {
		if (!N) return;
		AXE_DUMP_VAR(sz_, N);

		MyString tmp;
		UtfUtil::convert(tmp, sz_);

		MyString str(tmp.c_str());
		MyStrView view(tmp.c_str());

		MyChar ch[N - 1];
		my_memcpy(ch, tmp);

		AXE_TEST_CHECK(str == ch);

		AXE_TEST_CHECK(view == ch);

		AXE_TEST_CHECK(ch == str);
		AXE_TEST_CHECK(ch == view);
	}

	template<size_t N>
	void test_compare(const wchar_t (&sz_)[N]) {
		if (!N) return;

		AXE_DUMP_VAR("sz_", N);
//		AXE_DUMP_VAR(sz_, N); need to support Fmt print wchar_t*

		MyString tmp;
		UtfUtil::convert(tmp, sz_);

		MyString str(tmp.c_str());
		MyStrView view(tmp.c_str());

		MyChar ch[N - 1];
		my_memcpy(ch, tmp);

		AXE_TEST_CHECK(str == ch);

		AXE_TEST_CHECK(view == ch);

		AXE_TEST_CHECK(ch == str);
		AXE_TEST_CHECK(ch == view);
	}
};

} // ----- end of namespace -----

#define AXE_TEST_STRING_CASE(...) \
	AXE_TEST_CASE(Test_String< StringA >, __VA_ARGS__); \
	AXE_TEST_CASE(Test_String< StringW >, __VA_ARGS__); \
//---

void test_String() {
	using namespace axe;
	AXE_TEST_STRING_CASE(test_compare("Hello World!"));
	AXE_TEST_STRING_CASE(test_compare(L"Hello World!"));

	{
		const char sz[] = "Hello\0";
		constexpr static size_t N = sizeof(sz) / sizeof(sz[0]);
		AXE_TEST_STRING_CASE(test_compare<N>(sz));
	}
	{
		const wchar_t sz[] = L"Hello\0";
		constexpr static size_t N = sizeof(sz) / sizeof(sz[0]);
		AXE_TEST_STRING_CASE(test_compare<N>(sz));
	}
}
