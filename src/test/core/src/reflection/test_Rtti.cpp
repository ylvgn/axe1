
#include <axe_core/base/UnitTest.h>
#include <axe_core/base/Reflection.h>

namespace axe {

class Test_Rtti : public UnitTestBase {
public:
	template<class T>
	class Foo {
	public:
		int x, y;
		AXE_META_TYPE(Foo, NoBaseClass) {
			AXE_META_FIELD(x) {};
			AXE_META_FIELD(y) {};
			using OwnFields = Tuple<x,y>;
		};
	};

	template<class T, Int N>
	class Bar : public Foo<T> {
		AXE_META_TYPE_EX(Bar, Foo<T>)
	public:
		int bar;
	}; 

	//template<class T, float N> // require c++20
	template<class T, int N>
	class Bar2_NoInitMetaType : public Foo<T> {
		AXE_META_TYPE(Bar2_NoInitMetaType, Foo<T>) {};
	public:
		int bar2;
	}; 
	
	class MyObject : public RttiObject {
		AXE_RTTI_INFO(MyObject, RttiObject)
	};
	
	void test_case1();
};

//template<class T, Int N>
//struct Test_Rtti::Bar<T, N>::MetaTypeInit : AX_META_TYPE_INIT(AX_WRAP(Bar<T,N>)) {

template<class T, Int N>
struct Test_Rtti::Bar<T, N>::MetaTypeInit : public MetaTypeInit_Helper_<_TYPE_INFO_This> {
//	static NameId s_name() { return NameId("Bar"); }
	AXE_META_FIELD(bar) {};
	using OwnFields = Tuple<bar>;
};

void Test_Rtti::test_case1() {
	{
		Rtti* ti = rttiOf< Foo<void> >();
//		AXE_DUMP_VAR(*ti);
		AXE_TEST_CHECK(ti->allFields.size() == 2);
		AXE_TEST_CHECK(ti->allFields[0]->name == "x");
		AXE_TEST_CHECK(ti->allFields[1]->name == "y");
	}
		
	{
		Rtti* ti = rttiOf< Bar<StrView, 99> >();
//		AXE_DUMP_VAR(*ti);
		AXE_TEST_CHECK(ti->allFields.size() == 3);
		AXE_TEST_CHECK(ti->allFields[0]->name == "x");
		AXE_TEST_CHECK(ti->allFields[1]->name == "y");
		AXE_TEST_CHECK(ti->allFields[2]->name == "bar");
		// own fields
		AXE_TEST_CHECK(ti->ownFields[0]->name == "bar");
	}

	{
		//Rtti* ti = rttiOf< Bar2_NoInitMetaType<float, 1.1f> >(); require c++20
		Rtti* ti = rttiOf< Bar2_NoInitMetaType<float, 1> >();
//		AXE_DUMP_VAR(*ti);
		AXE_TEST_CHECK(ti->allFields.size() == 2);
		AXE_TEST_CHECK(ti->allFields[0]->name == "x");
		AXE_TEST_CHECK(ti->allFields[1]->name == "y");
	}
	
	{
		Rtti* ti = rttiOf<MyObject>();
		AXE_TEST_CHECK(ti->name == "class axe::Test_Rtti::MyObject");
	}
}

} // namespace axe

void test_Rtti() {
	using namespace axe;
	AXE_TEST_CASE(Test_Rtti, test_case1());
}
