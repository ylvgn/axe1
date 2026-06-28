#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_Vector : public UnitTestBase {
public:
	void test_move() {
		// Vector<int> arr1 = {1, 2, 3, 4, 5}; Not allow atm

		Vector<int> arr1;
		arr1.resize(5);
		for (int i = 0; i < arr1.size(); i++)
			arr1[i] = i;

		AXE_TEST_CHECK(arr1.size() > 0);

		static const int kArr2Size = 3;
		Vector<int> arr2;
		arr2.reserve(kArr2Size);
		for (int i = 0; i < kArr2Size; i++)
			arr2.emplace_back(i + 100);

		int arr2_0 = arr2[0];
		int arr2_1 = arr2[1];
		int arr2_2 = arr2[2];

		arr1.move(arr2);
		AXE_TEST_CHECK(arr1.size() == kArr2Size);
		AXE_TEST_CHECK(arr2.size() == 0);

		AXE_TEST_CHECK(arr1[0] == arr2_0);
		AXE_TEST_CHECK(arr1[1] == arr2_1);
		AXE_TEST_CHECK(arr1[2] == arr2_2);

		Vector<int> arr3(AXE_MOVE(arr1));
		AXE_TEST_CHECK(arr3.size() == kArr2Size);
		AXE_TEST_CHECK(arr1.size() == 0);
		AXE_TEST_CHECK(arr3[0] == arr2_0);
		AXE_TEST_CHECK(arr3[1] == arr2_1);
		AXE_TEST_CHECK(arr3[2] == arr2_2);
	}

	void test_appendRange() {
		Vector<int> arr1;
		arr1.resize(10);
		for (int i = 0; i < arr1.size(); i++)
			arr1[i] = i;

		AXE_TEST_CHECK(arr1.size() > 0);

		Vector<int> arr2;
		arr2.appendRange(arr1);
		AXE_TEST_CHECK(arr1.size() == arr2.size());
		AXE_TEST_CHECK(arr2[0] == arr1[0]);

		Vector<int> arr3;
		arr3.appendRange(AXE_MOVE(arr1));

		AXE_TEST_CHECK(arr1.size() == 0);
		AXE_TEST_CHECK(arr3.size() == arr2.size());
		AXE_TEST_CHECK(arr2[0] == arr3[0]);
	}

}; // Test_Vector

} // namespace axe


void test_Vector() {
	using namespace axe;

	AXE_TEST_CASE(Test_Vector, test_move());
	AXE_TEST_CASE(Test_Vector, test_appendRange());
}
