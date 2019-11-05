#include "solve_quadratic.c"
#define INIT_PRINT(A, B, C) printf("Coefficients: a = " #A ", b = " #B ", c = " #C "\n");

#define COMPARE(A) 	if((ABS(x1.re - x1_exp.re) < EPSILON) && (ABS(x1.im - x1_exp.im) < EPSILON)\
						&& (ABS(x2.re - x2_exp.re) < EPSILON) && (ABS(x2.im - x2_exp.im) < EPSILON)) \
						printf("Test " #A " passed\n"); \
					else{ \
						printf("Test " #A " failed!\n"); \
						print_complex_number(x1); \
						print_complex_number(x2); \
						printf("\n\nExpected:\n"); \
						print_complex_number(x1_exp); \
						print_complex_number(x2_exp); \
						printf("\n"); \
					}
void unit_test1(){
	printf("***Unit test 1***\n");
	complex_number x1 = {0, 0};
	complex_number x2 = {0, 0};
	INIT_PRINT(0, 0, 0)
	complex_number x1_exp = {0, 0};
	complex_number x2_exp = {0, 0};

	printf("Result:\n");
	solve_quadratic(0, 0, 0, &x1, &x2);
	COMPARE(1)
	printf("\n\n");
}

void unit_test2(){
	printf("***Unit test 2***\n");
	complex_number x1 = {0, 0};
	complex_number x2 = {0, 0};
	
	INIT_PRINT(1, 5, -6);
	complex_number x1_exp = {1, 0};
	complex_number x2_exp = {-6, 0};
	
	printf("Result:\n");
	solve_quadratic(1, 5, -6, &x1, &x2);
	COMPARE(2)
	printf("\n\n");
}

void unit_test3(){
	printf("***Unit test 3***\n");
	complex_number x1 = {0, 0};
	complex_number x2 = {0, 0};
	
	INIT_PRINT(0, 10, -5);
	complex_number x1_exp = {0.5, 0};
	complex_number x2_exp = {0, 0};
	
	printf("Result:\n");
	solve_quadratic(0, 10, -5, &x1, &x2);
	COMPARE(3)
	printf("\n\n");
}

void unit_test4(){
	printf("***Unit test 4***\n");
	complex_number x1 = {0, 0};
	complex_number x2 = {0, 0};
	
	INIT_PRINT(0.5, -0.32, 0.0512);
	complex_number x1_exp = {0.32, 0};
	complex_number x2_exp = {0.32, 0};
	
	printf("Result:\n");
	solve_quadratic(0.5, -0.32, 0.0512, &x1, &x2);
	COMPARE(4)
	printf("\n\n");
}

void unit_test5(){
	printf("***Unit test 5***\n");
	complex_number x1 = {0, 0};
	complex_number x2 = {0, 0};
	
	INIT_PRINT(2, 10, 17);
	complex_number x1_exp = {-2.5, 1.5};
	complex_number x2_exp = {-2.5, -1.5};
	
	printf("Result:\n");
	solve_quadratic(2, 10, 17, &x1, &x2);
	COMPARE(5)
	printf("\n\n");
}

int main(){
	unit_test1();
	unit_test2();
	unit_test3();
	unit_test4();
	unit_test5();
	return 0;
}
