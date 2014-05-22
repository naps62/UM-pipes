/**
 * so para testes
 */

#define _DEBUG 1

#include <stdio.h>

#include "linked_list.h"

void *test_getKey(void *data) {
	return data;
}

int test_compareKeys(void *data1, void *data2) {
	int v1 = (*(int *)data1);
	int v2 = (*(int *)data2);
	return v2 - v1;
}

void printInt(void *x) {
	printf("%d", *(int *)x);
}

void* coiso(void *data, void *recResult, void *params) {
	int *res = (int *) params;
	*res = *(int *)data + *(int *)recResult;
	return res;
}

int filter(void *data, void *params) {
	params = params;
	return (*(int *)data) == 1;
}

void teste_linked_list() {
	List list = list_create(&test_getKey, &test_compareKeys, LIST_NO_FREE);
	int zero=0, a=1, b=2, c=3, d=4, e=5;
	list_push(list, &e);
	list_push(list, &d);
	list_push(list, &c);
	list_push(list, &b);
	list_push(list, &a);
	list_remove(list, &e);
	list_remove(list, &d);
	printf("%d\n", *(int *)list_last(list));

	printf("%d\n", list_size(list));
}

int main() {
	teste_linked_list();
	return 0;
}
