#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_StrView : public UnitTestBase {
public:	
	StrView f1() {
		return StrView();
	}

	StrView f2() {
		return "";
	}

	StrView f3() {
		return String();
	}

	void test_1() {
		if (f1() == f2()) {
			AXE_DUMP_VAR("f1 == f2"); // error
		}

		if (f2() == f3()) {
			AXE_DUMP_VAR("f2 == f3"); // error
		}

		if (f1() == f3()) {
			AXE_DUMP_VAR("f1 == f3"); // error
		}
	}

	void test_2() {
		String str = "hello \"world";

		for (auto& s : str) {
			if (s == '"') {
				AXE_LOG("1111111111");
			}

			if (s == '\"') {
				AXE_LOG("22222222");
			}
		}
	}

	void test_3() {
		
		String test = "f 1 2 3 4";
		
		for (int i = 0; i < test.size(); i += 2)
		{
			StrView _token(test.begin() + i, 1);

			AXE_LOG("token={}", _token);
			int vi = 0, vt = 0, vn = 0;

			auto pair = StringUtil::splitByChar(_token, '/');
			if (!StringUtil::tryParse(pair.first, vi)) {
				AXE_LOG("====vi====>{}", _token);
			}

			//vt
			pair = StringUtil::splitByChar(pair.second, '/');
			if (pair.first.size()) {
				if (!StringUtil::tryParse(pair.first, vt)) {
					AXE_LOG("====vt====>{}", _token);
				}
			}
			//vn
			pair = StringUtil::splitByChar(pair.second, '/');
			if (pair.first.size()) {
				if (!StringUtil::tryParse(pair.first, vn)) {
					AXE_LOG("====vn====>{}", _token);
				}
			}

			AXE_LOG("vi={} vt={} vn={}", vi, vt, vn);
		}
	}

	template<size_t N>	static void foo(String_<N>& s)	{ AXE_LOG("foo String_: {}", s); }
	template<size_t N>	static void foo(StringW_<N>& s) { wprintf(L"foo StringW_: %ws", s.c_str()); }
						static void foo(String& s)		{ AXE_LOG("foo String: {}", s); }
						static void foo(StringW& s)		{ wprintf(L"foo StringW: %ws", s.c_str()); }

	void test_4() {
		String s1 = "s1 - String";
		foo(s1);

		TempString s2 = "s2 - TempString";
		foo(s2);

		StringW s3 = L"s3 - StringW";
		foo(s3);

		String_<1> s4 = "s4 - String_1";
		foo(s4);

		StringW_<1> s5 = L"s5 - StringW_1";
		foo(s5);
	}

	void test_5() {
		char ch1[] = "char[]";
		StrView s1(ch1);
		AXE_LOG("{}, len={}", s1, s1.size());

		char ch2[20];
		ch2[0] = 'c';
		ch2[1] = 'h';
		ch2[2] = 'a';
		ch2[3] = 'r';
		ch2[4] = '[';
		ch2[5] = ']';
		ch2[6] = 0;

		StrView s2(ch2);
		AXE_LOG("{}, len={}", s2, s2.size());

		const char* sz = "string literal";
		StrView s3(sz);
		AXE_LOG("{}, len={}", s3, s3.size());

		std::string stdStr = "std::string";
		StrView s4(stdStr.c_str());
		AXE_LOG("{}, len={}", s4, s4.size());

		TempString tmpStr = "TempString";
		StrView s5(tmpStr);
		AXE_LOG("{}, len={}", s5, s5.size());
	}

	template<size_t N> inline
	static void foo2(const char (&ch)[N]) { AXE_LOG("{}, N={}", ch, N); }

	void test_6() {
		foo2("hello1");

		const char s2[] = "hello2";
		foo2(s2);

		const char s3[20] = "hello3";
		foo2(s3);
	}
};

} // namespace 

void test_StrView() {
	using namespace axe;
	//AXE_TEST_CASE(Test_StrView, test_1());
	//AXE_TEST_CASE(Test_StrView, test_2());
	//AXE_TEST_CASE(Test_StrView, test_3());
	//AXE_TEST_CASE(Test_StrView, test_4());
	//AXE_TEST_CASE(Test_StrView, test_5());
	AXE_TEST_CASE(Test_StrView, test_6());
}
