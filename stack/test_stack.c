#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define unit_test(number, res, exp) \
									if ((res) != exp) \
										printf("Unit test " #number " failed!\n"); \
									else \
										printf("Unit test " #number " is passed\n") \


#define MODULE(n); printf("\nTest module %d \n", n);

void test_stack_create(int n){
	MODULE(n);

	Stack *stk = stack_create("file.log", "stk");

	unit_test(1, stack_ok(stk), 0);

	stack_destroy(stk);
}

void test_stack_set_size(int n){
	MODULE(n);

	Stack *stk  = stack_create("file.log", "stk");
	
	data_t x = 0;
	
	stack_set_size(stk, 30);

	stack_set_size(stk, 95);

	stack_set_size(stk, 1000);

	unit_test(1, stk->capacity, 1000);

	stack_set_size(stk, 37);

	unit_test(2, stk->capacity, 37);

	stack_push(stk, 25);

	stack_push(stk, 45);

	stack_set_size(stk, 100);

	stack_pop(stk, &x);

	unit_test(3, x, 45);

	stack_set_size(stk, -1);

	unit_test(4, stk->errno, INVALID_CAPACITY_ERR);

	stack_destroy(stk);
}

void test_stack_push_pop(int n){
	MODULE(n);

	Stack *stk  = stack_create("file.log", "stk");

	data_t x = 0;
	
	stack_dump(stk);

	for(int i = 0; i <= 100; i++){
		stack_push(stk, i);
	}

	for(int i = 0; i <= 100; i++){
		stack_pop(stk, &x);
	}
	
	stack_dump(stk);

	unit_test(1, x, 0);

	stack_pop(stk, &x);

	unit_test(2, stk->errno, UNDERFLOW_ERR);

	stack_destroy(stk);
}


void test_stack_hash(int n){
	MODULE(n);

	Stack *stk = stack_create("file.log", "stk");
	
	long int hash = stk->hash;

	data_t x = 0;

	stack_push(stk, 25);
	
	stack_pop(stk, &x);

	unit_test(1, stack_hash(stk), hash);

	hash = stk->hash;

	stack_hash(stk);

	stack_hash(stk);

	stack_hash(stk);

	unit_test(2, stack_hash(stk), hash);

	stack_destroy(stk);
}


void test_stack_ok(int n){
	MODULE(n);

	Stack *stk = stack_create("file.log", "stk");	

	free(stk->data);

	stk->data = NULL;

	stack_ok(stk);

	unit_test(1, stk->errno, INVALID_DATA_PTR_ERR);

	stack_destroy(stk);

	stk = stack_create("file.log", "stk");

	*(char*)(stk->data + STACK_INIT_SIZE) = 37;

	stack_ok(stk);

	unit_test(2, stk->errno, INCORRECT_HASH_ERR);

	stack_destroy(stk);

	stk = stack_create("file.log", "stk");

	stk->data[sizeof(stack_data_canary) / 2] = 25;

	stack_ok(stk);

	unit_test(3, stk->errno, DATA_CANARY_DEAD);

	stack_destroy(stk);

	stk = stack_create("file.log", "stk");

	stk->capacity = -7;

	stack_push(stk, 10);

	unit_test(4, stk->errno, INVALID_CAPACITY_ERR);

	stack_destroy(stk);
}

void test_stack_clear(){
	MODULE(6);

	Stack *stk  = stack_create("file.log", "stk");

	stack_push(stk, 1);
	
	stack_push(stk, 6);

	stack_push(stk, 10);

	stack_clear(stk);

	unit_test(1, stack_ok(stk), 0);

	unit_test(2, stk->size, 0);

	data_t x = 0;

	stack_pop(stk, &x);

	unit_test(3, stk->errno, UNDERFLOW_ERR);

	stack_destroy(stk);
}


/*
void test_stack_create(){
	MODULE(1);

	Stack *stk = stack_create("file.log", "stk");

	unit_test(1, stack_ok(stk), 0);

	stack_destroy(stk);
}
*/

int main(){
	
	test_stack_create(1);
	test_stack_set_size(2);
	test_stack_push_pop(3);
	test_stack_hash(4);
	test_stack_ok(5);
	test_stack_clear(6);

	return 0;
}
