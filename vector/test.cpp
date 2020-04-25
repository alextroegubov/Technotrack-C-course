
int module_n_ = 1;
//int test_n_ = 1;
bool res_ = false;

#define TEST(smth) 	 printf("\nTest module %d : testing %s\n", module_n_++, #smth);
#define ASSERT(arg1, arg2) 	res_ = (arg1 == arg2); \
							printf("\t[%3d] $%s == %s$: %s", __LINE__, #arg1, #arg2, res_? "ok" : "ASSERTION FAILED!"); \
							if(res_ == false){ \
								std::cout << ": arg1 = " << arg1 << " " << "arg2 = " << arg2; \
							} \
							printf("\n\n");\

#include "vector.h"

void Test1(){
	TEST(constructors and operators =);

	Vector v1(3);
	ASSERT(v1.GetSize(), 0);
	ASSERT(v1[0] + v1[1] + v1[2], 3 * POISON_VALUE);

	Vector v2(5, 1);
	ASSERT(v2.GetSize(), 5);
	ASSERT(v2[4], 1);

	Vector v3(v2);
	ASSERT(v3.GetSize(), 5);
	ASSERT(v3[4], 1);

	v1 = v2;
	ASSERT(v1.GetSize(), 5);
	ASSERT(v1[4], 1);

	Vector v5(10);
	v5 = std::move(v2);
	ASSERT(v5.GetSize(), 5);
	ASSERT(v5[4], 1);

	Vector v6(std::move(v3));
	ASSERT(v6.GetSize(), 5);
	ASSERT(v6[4], 1);
}

void Test2(){
	TEST(Resize);

	Vector v1(10, 1);

	v1.Resize(5);
	ASSERT(v1.GetSize(), 5);
	ASSERT(v1[4], 1);

	v1.Resize(10);
	ASSERT(v1[8], POISON_VALUE);
	ASSERT(v1.GetSize(), 5);

	Vector v2(10);
	ASSERT(v2.GetSize(), 0);

	v2.Resize(100);
	ASSERT(v2.GetSize(), 0);

	v2.Resize(2);
	ASSERT(v2.GetSize(), 0);
}

void Test3(){
	TEST(PushBack);
	Vector v1(2);
	v1.PushBack(1);
	v1.PushBack(2);
	v1.PushBack(3);
	v1.PushBack(4);

	ASSERT(v1[0], 1);
	ASSERT(v1[1], 2);
	ASSERT(v1[2], 3);
	ASSERT(v1[3], 4);
}

void Test4(){
	TEST(PopBack);
	Vector v1(2, 1);
	v1.PopBack();
	v1.PopBack();
	ASSERT(v1.GetSize(), 0);
}

int main(){
	Test1();
	Test2();
	Test3();
	Test4();

	return 0;
}