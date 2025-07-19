#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_Tuple : public UnitTestBase {
	using This = Test_Tuple;
public:

	struct ForEachType_Handler {
		void _todo() {
			AXE_TODO("AXE_DUMP_VAR(index, TypeOf<Field>()->name)")
		}

		template <int index, class Field>
		void handle() {
			_todo();
			AXE_DUMP_VAR(index);
		}
	};

	void test() {
		auto tupleA = Tuple_make(100U, 1.23, 4.5f, -10, StrView("test"), String("abc"));
		AXE_DUMP_VAR(tupleA);

		auto tupleB = Tuple_make(400, 500);
		AXE_DUMP_VAR(tupleB);
		AXE_TEST_CHECK(tupleB.get<0>() == 400);
		AXE_TEST_CHECK(tupleB.get<1>() == 500);

		auto tupleAB = tupleA.join(tupleB);
		AXE_DUMP_VAR(tupleAB);
		AXE_TEST_CHECK(tupleAB.size() == tupleA.size() + tupleB.size());

		AXE_TEST_CHECK(tupleAB.get<0>() == 100U);
		AXE_TEST_CHECK(tupleAB.get<1>() == 1.23);
		AXE_TEST_CHECK(tupleAB.get<2>() == 4.5f);
		AXE_TEST_CHECK(tupleAB.get<3>() == -10);
		AXE_TEST_CHECK(tupleAB.get<4>() == "test");
		AXE_TEST_CHECK(tupleAB.get<5>() == "abc");
		// from B
		AXE_TEST_CHECK(tupleAB.get<6>() == 400);
		AXE_TEST_CHECK(tupleAB.get<7>() == 500);

		tupleAB.forEach([](auto index, const auto& value) {
			AXE_DUMP_VAR(value);
		});

		ForEachType_Handler h;
		decltype(tupleAB)::s_forEachType(h);
	}
}; // Test_Tuple

} // namespace axe


void test_Tuple() {
	using namespace axe;

	AXE_TEST_CASE(Test_Tuple, test());
}
