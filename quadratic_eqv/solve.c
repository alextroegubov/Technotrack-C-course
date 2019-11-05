#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <assert.h>
#include <ctype.h>

#define NO_ROOTS 0
#define ONE_ROOT 1
#define INF_ROOTS 2
#define TWO_REAL_ROOTS 3
#define TWO_COMPLEX_ROOTS 4
#define MATCHING_ROOTS 5
#define DEFAULT -1


#define EPSILON 0.000005D 
#define ABS(a) (a > 0 ? a : -(a))

#define BUFFER_SIZE 10


struct complex_number {
	double re;
	double im;
};
typedef struct complex_number complex_number;

short int read_coefficients(double *a, double *b, double *c);
int solve_linear(double b, double c, complex_number *x1);
int read_number(double *number);
int solve_quadratic(double a, double b, double c, complex_number *x1, complex_number *x2);
void print_complex_number(complex_number x);

int main(){

	printf("The program solves quadratic equation\n"
			"a*x^2 + b*x + c = 0\n\n");
	printf("Enter the value of rational coefficients  a, b ,c:\n");

	double a = 0, b = 0, c = 0;

	while(read_coefficients(&a, &b, &c))
			printf("Inappropriate values! Try again: \n");

	printf("Coefficients: a = %lf, b = %lf, c = %lf \n", a, b, c);

	complex_number x1 = {0, 0}, x2 = {0, 0}; 	// roots in complex form
	int nroots = DEFAULT; 						//the number of roots
	
	nroots = solve_quadratic(a, b, c, &x1, &x2);

	switch(nroots){

	case NO_ROOTS:
		printf("No roots\n");
		break;

	case ONE_ROOT:
		printf("One root: x = %lf\n", x1.re);
		break;

	case INF_ROOTS:
		printf("Infinite number of roots\n");
		break;

	case TWO_REAL_ROOTS:
		printf("Two real roots: x1 = %lf, x2 = %lf\n", x1.re, x2.re);
		break;

	case TWO_COMPLEX_ROOTS:
		printf("Two complex roots: ");
		print_complex_number(x1);
		print_complex_number(x2);
		printf("\n");
		break;

	case MATCHING_ROOTS:
		printf("Two matched roots: x1 = x2 = %lf\n", x1.re);
		break;

	case DEFAULT:

	default:
		printf("Error in switch, nroots = %d\n", nroots);
	}

	return 0;
}


/**
 * Function reads 3 double coefficients from stdin and puts in addresses pointed by a, b and c. If an error occurs, 
 * stdin will be freed.
 *
 * @param [in] a - a pointer to the first coefficient
 * @param [in] b - a pointer to the secong coefficient
 * @param [in] c - a pointer to the third coefficient
 *
 * @return In success returns zero, in other cases returns non-zero values. 
*/
short int read_coefficients(double *a, double *b, double *c){

	char *rubbish_buf = malloc(1000 * sizeof(char)); 
	if(!rubbish_buf){
		printf("read_coefficients: Error while allocation!\n");
		return 1;
	}

	if(read_number(a) || read_number(b) || read_number(c)){
		*a = 0;
		*b = 0;
		*c = 0;
		fgets(rubbish_buf, 1000, stdin);
		free(rubbish_buf);
		return 2;
	}

	free(rubbish_buf);
	return 0;
}

/**
 * Function solves linear equation b*x + c = 0 with real coefficiants b and c. Puts solution (if any) in adress 
 * pointed by x1. In case of no solution function does not change anything. 
 *
 * @param [in] b - the first coefficient (next to x)
 * @param [in] c  - the second coefficient
 * @param [out] x1 -  a pointer to struct complex_number

 * @return Returns INF_ROOTS, ONE_ROOT, NO_ROOTS in case of infinite number of roots,
 * one root or no roots at all. 
*/
int solve_linear(double b, double c, complex_number *x1){
	
	assert(x1 != NULL);
	assert(isfinite((float)b));
	assert(isfinite((float)c));

	if (b == 0){
		return (c == 0) ? INF_ROOTS : NO_ROOTS;
	} else{
		x1->re = -c/b;
		return ONE_ROOT;
	}
}


/**
 * Function read double number from stdin an puts in to adress pointed by number
 *
 * @param [out] number -  a pointer to double
 *
 * @return Returns 0 in case of succefful reading and 1 in other cases.
*/
int read_number(double *number){
	
	assert(number != NULL);

	char buffer[BUFFER_SIZE] = {'\0'};
	char sym = getchar();
	char *end_ptr = &sym;	//current symbol
	int index = 0;			//current index for free space in buffer

	while(isspace(sym))
		sym = getchar();
			
	while(!isspace(sym) && index < BUFFER_SIZE){
		buffer[index++] = sym;
		sym = getchar();
	}

	*number = strtod(buffer, &end_ptr);
	
	if((*end_ptr) != buffer[index])
		return 1;
	else 
		return 0;
}


/**
 * Function solves quadratic equation a*x^2 + b*x + c = 0 with real coefficiants a, b and c. Puts solution (if any) in 
 * adresses pointed by x1 and x2. x1 and x2 are set to 0 before calculating. 
 *
 * @param [in] a - the first coefficient (next to x^2)
 * @param [in] b - the second coefficient (next to x)
 * @param [in] c  - the third coefficient
 * @param [out] x1 -  a pointer to struct complex_number (the first root)
 * @param [out] x2 - a pointer to struct complex_number (the second roor)
 *
 * @return Returns the number of roots: INF_ROOTS, ONE_ROOT, NO_ROOTS,  TWO_COMPLEX_ROOTS, TWO_REAL_ROOTS or 
 * MATCING_ROOTS  
*/
int solve_quadratic(double a, double b, double c, complex_number *x1, complex_number *x2){
	
	assert(x1 != x2);
	assert(x1 != NULL);
	assert(x2 != NULL);
	assert(isfinite((float)a));
	assert(isfinite((float)b));
	assert(isfinite((float)c));

	x1->re = 0;
	x1->im = 0;
	x2->re = 0;
	x2->im = 0;

	//b*x + c = 0
	if (a == 0)
		return  solve_linear(b, c, x1); 

	//a*x^2 + b*x + c = 0   
	if (b == 0 && c == 0)
		return MATCHING_ROOTS;
	
	double D = b * b - 4 * a * c;

	if(ABS(D) < EPSILON){
		x1->re = -b / (2 * a);
		x2->re = x1->re;
		return MATCHING_ROOTS;
	} else if(D > 0){
		D = sqrt(D);
		x1->re = (-b + D) / (2 * a);
		x2->re = (-b - D) / (2 * a);
		return TWO_REAL_ROOTS;
	} else{
		D = sqrt(-D);
		x1->re = -b / (2 * a);
		x1->im = D / (2 * a);
		x2->re = x1->re;
		x2->im = -x1->im;
		return TWO_COMPLEX_ROOTS;
	}
}

/**
 * Function prints complex number x in fortmat Re(x) + Im(x) * i. If Re(x) = Im(x) = 0 than prints '0'. If Re(x) = 0 /
 * Im(x) = 0 it does not print this part it.
 *
 * @param [in] x1 - struct complex_number
 *
 * @return nothing
*/
void print_complex_number(complex_number x){
	if(ABS(x.re) < EPSILON  &&  ABS(x.im) < EPSILON) 
		printf("0");
	else if(ABS(x.im) < EPSILON)
		printf("%lf ", x.re);
	else if(ABS(x.re) < EPSILON)
		printf("%lf*i ", x.im);
	else if(x.im > EPSILON) //x.im > 0
		printf("%lf+%lf*i ", x.re, x.im);
	else 
		printf("%lf%lf*i ", x.re, x.im);
}

