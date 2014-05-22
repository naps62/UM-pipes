#include <stdlib.h>
#include <stdio.h>

#include "defs.h"
#include "linked_list.h"
#include "taskManager.h"

//<editor-fold defaultstate="collapsed" desc="Private Datatypes">

struct list_elem {
	void *data;
	struct list_elem *next;
};
typedef struct list_elem *ListElem;

struct linked_list {
	ListElem head; /**< Element list */
	ListElem last; /**< last element */
	void* (*getKey)(void*); /**< Function to extract the key from data */
	int (*compareKeys)(void*, void*); /**< Function to compare two keys, and return -1, 0 or 1 */
	void* (*destructor)(void* data, void* params); /**< Called to each data when removed */
	int totalCount; /**< Current element count */
};
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="List Creation">

List list_create(void* (*getKey)(void*), int (*compareKeys)(void*, void*), void* (*destructor)(void *data, void *params)) {
	List ret = (List) malloc(sizeof (struct linked_list));
	ret->compareKeys = compareKeys;
	ret->getKey = getKey;
	ret->destructor = destructor;
	ret->head = NULL;
	ret->last = NULL;
	ret->totalCount = 0;

	return ret;
}

int list_setDestructor(List list, void* (*newDestructor)(void *data, void* params)) {
	list->destructor = newDestructor;
	return 0;
}

void* callDestructor(List list, void* (*destructor)(void *data, void *params), void *data, void *params) {
	if (destructor == NULL) {
		if (list->destructor != NULL)
			return list->destructor(data, params);
		return NULL;
	} else if (destructor == LIST_SIMPLE_FREE) {
		free(data);
		return NULL;
	} else if (destructor == LIST_NO_FREE) {
		return NULL;
	} else {
		return destructor(data, params);
	}
}
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="List Management">

int list_isEmpty(List list) {
	return (list->totalCount == 0);
}

int list_hasElems(List list) {
	return (list->totalCount != 0);
}

int list_size(List list) {
	return (list->totalCount);
}
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="Insertion & Removal Functions">

int list_insertHead(List list, void *newData) {
	ListElem newElem = (ListElem) malloc(sizeof (struct list_elem));

	newElem->data = newData;
	newElem->next = list->head;

	list->head = newElem;

	if (list->totalCount == 0)
		list->last = newElem;

	list->totalCount++;
	return 0;
}

int list_insertTail(List list, void *newData) {
	ListElem newElem = (ListElem) malloc(sizeof (struct list_elem));

	newElem->data = newData;
	newElem->next = NULL;

	if (list->last == NULL) {
		list->head = newElem;
		list->last = newElem;
	} else {
		list->last->next = newElem;
		list->last = newElem;
	}

	list->totalCount++;
	return 0;
}

int list_insertAt(List list, void *newData, int pos) {
	ListElem *aux = &(list->head);

	pos--;
	while (*aux != NULL && pos > 0) {
		aux = &((*aux)->next);
		pos--;
	}

	ListElem newElem = (ListElem) malloc(sizeof (struct list_elem));
	newElem->data = newData;
	newElem->next = *aux;
	*aux = newElem;

	if (pos > list->totalCount)
		list->last = newElem;

	list->totalCount++;
	return 0;
}

int list_insertOrd(List list, void *newData) {
	ListElem *aux = &(list->head);

	while (*aux != NULL && list->compareKeys(list->getKey(newData), list->getKey((*aux)->data)) < 0) {
		aux = &((*aux)->next);
	}

	ListElem newElem = (ListElem) malloc(sizeof (struct list_elem));
	newElem->data = newData;
	newElem->next = *aux;
	*aux = newElem;

	list->totalCount++;
	return 0;
}

void* list_remove(List list, void *key) {
	ListElem *aux = &(list->head);
	ListElem prev = NULL;
	
	while (*aux != NULL && list->compareKeys(key, list->getKey((*aux)->data)) != 0) {
		prev = *aux;
		aux = &((*aux)->next);
	}
	void* ret = (*aux)->data;
	ListElem toFree = *aux;
	(*aux) = (*aux)->next;
	free(toFree);
	list->totalCount--;
	
	
	if (list->last == list->head) {
		list->head = NULL;
		list->last = NULL;
	}
	else if (list->last == toFree) {
		list->last = prev;
	}

	return ret;
}

void* list_removeIndex(List list, int index) {
	ListElem *aux = &(list->head);

	while (*aux != NULL && index > 0) {
		aux = &((*aux)->next);
	}
	void* ret = (*aux)->data;
	ListElem toFree = *aux;
	(*aux) = (*aux)->next;
	free(toFree);

	return ret;
}
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="Stack Functions">

int list_push(List list, void *newData) {
	return list_insertHead(list, newData);
}

void* list_pop(List list) {
	if (list->totalCount == 0) {
		return NULL;
	}

	void *ret = list->head->data;
	ListElem toFree = list->head;
	list->head = list->head->next;

	if (list->totalCount == 1) {
		list->last = NULL;
	}

	free(toFree);

	list->totalCount--;
	return ret;
}

void* list_top(List list) {
	if (list->totalCount == 0)
		return NULL;
	return list->head->data;
}

void* list_last(List list) {
	if (list->totalCount == 0)
		return NULL;
	return list->last->data;
}
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="Search Functions">

void* list_search(List list, void* key) {
	ListElem aux = list->head;

	while (aux != NULL) {
		if (list->compareKeys(key, list->getKey(aux->data)) == 0)
			return aux->data;
		aux = aux->next;
	}

	return NULL;
}

void* list_searchByFunction(List list, int (*searchFunction)(void*, void*), void *params) {
	ListElem aux = list->head;

	while (aux != NULL) {
		if (searchFunction(aux->data, params))
			return aux->data;

		aux = aux->next;
	}

	return NULL;
}
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="List Mapping">

int list_map(List list, int (*function)(void *, void *), void *params) {
	ListElem aux = list->head;
	int ret = 0;

	while (aux != NULL) {
		ret += function(aux->data, params);
		aux = aux->next;
	}

	return ret;
}

List list_filter(List list, int (*function)(void *, void *), void *params) {
	List removed = list_create(list->getKey, list->compareKeys, list->destructor);

	ListElem *aux = &(list->head);
	ListElem *nextElem;
	while (*aux != NULL) {
		nextElem = &((*aux)->next);
		if (!function((*aux)->data, params)) {
			list_insertTail(removed, (*aux)->data);
			free(*aux);
			*aux = *nextElem;
			list->totalCount--;
		} else {
			aux = nextElem;
		}
	}

	return removed;
}

void *list_foldrAux(ListElem elem, void* (*func)(void *data, void *recResult, void *params), void *stopCase, void *params) {
	if (elem == NULL)
		return stopCase;

	return func(elem->data, list_foldrAux(elem->next, func, stopCase, params), params);
}

void *list_foldr(List list, void *(*func)(void *data, void *recResult, void *params), void *stopCase, void *params) {
	return list_foldrAux(list->head, func, stopCase, params);
}

//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="List Removal">
/**
 * @fn List_ClearAll
 * @brief Elimina toda a lista
 *
 * @param list Lista a eliminar
 * @return 0 em caso de sucesso
 */
int list_deleteAll(List list, void *params) {
	ListElem aux = list->head;

	while (aux != NULL) {
		list->head = aux->next;
		callDestructor(list, list->destructor, aux->data, params);
		aux = list->head;
	}
	free(list);

	return 0;
}

void *defDestructor(void *data, void *params) {
	free(data);
	return NULL;
}
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="DEBUG">

/**
 *
 * @fn list_Print
 * @brief Prints all list data, for debugging
 */
void list_print(List list, void (*printData)(void *)) {
	printf("Linked List:\n");
	printf("\tLength: %d\n", list->totalCount);
	printf("\tElems:\n");

	ListElem elem = list->head;
	if (printData) {
		while (elem) {
			printf("\tdata: ");
			printData(elem->data);
			printf("\tnext: %p\n", elem->next);
			elem = elem->next;
		}
	} else {
		while (elem) {
			printf("\tdata: %p\tnext: %p\n", elem->data, elem->next);
			elem = elem->next;
		}
	}
}
//</editor-fold>
