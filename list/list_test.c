#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define unit_test(number, res, exp) \
	if( (res) != (exp))\
		printf("Unit test " #number " failed!\n");\
	else\
		printf("Unit test " #number " is passed\n");

#define MODULE(n, name)\
	printf("\nTest module %d, testing " #name "\n", n);


void test1(){

	MODULE(1, list_create);

	int size = 10;

	List *lst = list_create("list.log", size);

	unit_test(1, lst && lst->next && lst-> prev, 1);

	unit_test(2, lst->next[size - 1], size);

	unit_test(3, lst->prev[size / 2], POISON_PHYS);

	unit_test(4, lst->hash, list_hash(lst));

	unit_test(5, lst->next[lst->capacity], POISON_PHYS);

	list_destroy(lst);
}

void test2(){
	
	MODULE(2, list_insert_after);

	int size = 10;

	int tmp = 0;

	List *lst = list_create("list.log", size);

	tmp = list_insert_after(lst, 1, 1);

	unit_test(1, tmp, POISON_PHYS);

	tmp = list_insert_after(lst, lst->head, 1);

	unit_test(2, tmp, 1);

	unit_test(3, lst->data[tmp], 1);

	tmp = list_insert_after(lst, lst->tail, 2);

	unit_test(4, lst->data[2], 2);

	list_insert_after(lst, lst->tail, 3);

	list_insert_after(lst, lst->tail, 4);

	list_insert_after(lst, lst->tail, 5);

	list_insert_after(lst, lst->head, -1);

	list_save_graph(lst, "graph.dot");

	unit_test(5, lst->head, 1);

	unit_test(6, lst->tail, 5);

	unit_test(7, lst->free, 7);

	unit_test(8, lst->data[lst->next[lst->head]], -1);

	list_destroy(lst);
}


void test3(){
	
	MODULE(3, list_insert_before);

	int size = 10;

	int tmp = 0;

	List *lst = list_create("list.log", size);

	tmp = list_insert_before(lst, 1, 1);

	unit_test(1, tmp, POISON_PHYS);

	tmp = list_insert_before(lst, lst->head, 1);

	unit_test(2, tmp, 1);

	unit_test(3, lst->data[tmp], 1);

	tmp = list_insert_before(lst, lst->tail, 2);

	unit_test(4, lst->data[lst->head], 2);

	list_insert_before(lst, lst->tail, 3);

	list_insert_before(lst, lst->tail, 4);

	list_insert_before(lst, lst->tail, 5);

	list_insert_before(lst, lst->head, -1);

	unit_test(5, lst->head, 6);

	unit_test(6, lst->tail, 1);

	unit_test(7, lst->free, 7);

	unit_test(8, lst->data[lst->head], -1);

	unit_test(9, lst->size, 6);

	list_destroy(lst);
}

void test4(){
	
	MODULE(4, list_delete);

	int size = 10;

	List *lst = list_create("list.log", size);

	list_insert_after(lst, lst->head, 1);

	list_insert_after(lst, lst->head, 2);

	list_insert_after(lst, lst->head, 3);

	list_insert_after(lst, lst->head, 4);

	list_insert_after(lst, lst->head, 5);

	list_delete(lst, lst->head);

	unit_test(1, lst->head, 5);

	list_delete(lst, lst->tail);

	unit_test(2, lst->tail, 3);

	unit_test(3, lst->free, 2);

	list_delete(lst, 4);

	unit_test(4, lst->next[4], 2);

	unit_test(5, lst->prev[4], POISON_PHYS); 

	unit_test(6, lst->size, 2);

	list_destroy(lst);
}

void test5(){
	
	MODULE(5, list_find_phys_by_log);

	int size = 10;

	List *lst = list_create("list.log", size);

	list_insert_after(lst, lst->head, 10);

	list_insert_after(lst, lst->head, 15);

	list_insert_after(lst, lst->head, 20);

	list_insert_after(lst, lst->head, 25);

	list_insert_after(lst, lst->head, 30);

	list_insert_after(lst, lst->head, 35);

	int hash = list_hash(lst);
	
	unit_test(1, list_find_phys_by_log(lst, 1), lst->head);

	unit_test(2, list_find_phys_by_log(lst, 6), lst->tail);

	unit_test(3, list_hash(lst), hash);

	list_destroy(lst);
}

void test6(){
	
	MODULE(6, list_hash);

	List *lst = list_create("list.log", 10);

	list_insert_after(lst, lst->tail, 1);

	list_insert_after(lst, lst->tail, 2);

	int hash = list_hash(lst);

	list_hash(lst);

	list_hash(lst);

	list_hash(lst);

	unit_test(1, hash, list_hash(lst));

	hash = list_hash(lst);

	list_insert_after(lst, lst->tail, 3);

	list_insert_after(lst, lst->tail, 4);

	list_delete(lst, lst->tail);

	list_delete(lst, lst->tail);

	unit_test(2, hash, list_hash(lst));

	list_destroy(lst);
}

void test7(){
	
	MODULE(7, list_ok);

	List *lst = list_create("list.log", 10);

	unit_test(1, list_ok(lst), 0);

	list_insert_after(lst, lst->tail, 7);

	list_insert_before(lst, lst->head, 8);

	list_insert_after(lst, 2, 9);

	unit_test(2, list_ok(lst), 0);

	list_delete(lst, lst->head);

	unit_test(3, list_ok(lst), 0);

	printf("list errno = %d\n", lst->errno);

	lst->free = 8;

	lst->next[8] = 9;

	lst->next[9] = 8;

	list_ok(lst);

	unit_test(4, lst->errno, HASH_ERR);

	lst->errno = NO_ERROR;

	list_hash(lst);

	list_ok(lst);

	unit_test(5, lst->errno, FREE_CIRCULARITY_ERR);

	printf("list errno = %d\n", lst->errno);

	list_destroy(lst);

	lst = list_create("list.log", 10);

	list_insert_after(lst, lst->head, 5);

	list_insert_after(lst, lst->tail, 6);

	list_insert_after(lst, lst->tail, 7);

	list_insert_after(lst, lst->tail, 8);

	lst->next[3] = 1;

	list_hash(lst);

	list_ok(lst);

	unit_test(6, lst->errno, HEAD_TAIL_CIRCULARITY_ERR);

	printf("list errno = %d\n", lst->errno);

	list_destroy(lst);
}


void test8(){

	MODULE(8, "list_sort");

	List *lst = list_create("list.log", 10);

	list_insert_after(lst, lst->tail, 1);

	list_insert_after(lst, lst->tail, 2);

	list_insert_after(lst, lst->tail, 3);

	list_insert_after(lst, lst->tail, 4);

	list_insert_after(lst, lst->tail, 5);

	list_insert_after(lst, lst->tail, 6);

	list_insert_after(lst, lst->tail, 7);

	list_insert_after(lst, lst->tail, 8);

	list_delete(lst, 4);

	list_delete(lst, 2);

	list_delete(lst, 7);

	list_insert_after(lst, lst->tail, 9);

	list_insert_after(lst, lst->tail, 10);

	list_insert_after(lst, lst->tail, 11);

	list_sort(lst);	

	unit_test(1, lst->data[3], 5);

	unit_test(2, lst->data[7], 10);

	list_delete(lst, 3);

	list_insert_before(lst, lst->head, 12);

	list_sort(lst);

	unit_test(3, list_find_phys_by_log(lst, 5), 5);

	list_insert_after(lst, 7, 13);

	list_sort(lst);

	unit_test(4, list_find_phys_by_log(lst, 8), 8);

	unit_test(5, list_ok(lst), 0);

	list_destroy(lst);
}

void test9(){
	
	MODULE(9, "list_increase_capacity");

	List *lst = list_create("list.log", 5);

	list_insert_after(lst, lst->tail, 1);

	list_insert_after(lst, lst->tail, 2);

	list_insert_after(lst, lst->tail, 3);

	list_insert_after(lst, lst->tail, 4);

	list_insert_after(lst, lst->tail, 5);

	list_increase_capacity(lst, 6);

	unit_test(1, list_ok(lst), 0);

	unit_test(2, lst->head, 1);

	unit_test(3, lst->tail, 5);

	unit_test(4, lst->data[5], 5);

	unit_test(5, lst->free, 6);

	list_insert_after(lst, 4, 6);

	list_insert_after(lst, 3, 7);

	unit_test(6, lst->size, 7);

	unit_test(7, list_ok(lst), 0);

	list_destroy(lst);
}

int main(){

	test1();

	test2();

	test3();

	test4();

	test5();

	test6();

	test7();

	test8();

	test9();

	return 0;
}
