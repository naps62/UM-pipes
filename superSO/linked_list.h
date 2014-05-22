/*
 * File:   structs.h
 * Author: Miguel Palhas
 */

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdlib.h>

/**/
//<editor-fold defaultstate="collapsed" desc="Public Datatypes">
#define LIST_NO_FREE     (void *) 0x1
#define LIST_SIMPLE_FREE (void *) 0x2

typedef struct linked_list *List;
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="List Creation">
/**
 * @fn list_create
 * @brief Creates a new list with default parameters
 * @return Created list
 */
List list_create(void* (*getKey)(void*data), int (*compareKeys)(void* key1, void* key2), void* (*destructor)(void *data, void *params));
int list_setDestructor(List list, void* (*newDestructor)(void *data, void* params));
void* callDestructor(List list, void* (*destructor)(void *data, void *params), void *data, void *params);
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="List Management">
int list_isEmpty(List list);
int list_hasElems(List list);
int list_size(List list);
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="Insertion & Removal Functions">
int list_insertHead(List list, void *newData); /**< Inserts in list head */
int list_insertTail(List list, void *newData); /**< Inserts in list tail */
int list_insertAt(List list, void *newData, int pos);
int list_insertOrd(List list, void *newData);

void* list_remove(List list, void *key);
void* list_removeIndex(List list, int index);
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="Stack Functions">
int list_push(List list, void *newData);
void* list_pop(List list);
void* list_top(List list);
void* list_last(List list);
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="Search Functions">
void* list_search(List list, void* key);
void* list_searchByFunction(List list, int (*searchFunction)(void*, void*), void *params);
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="List Mapping">
/**
 * @fn list_Map
 * @brief Applies a function to all data elements of the list
 *
 * @param list List to use
 * @param function Function to apply
 * @param params Parameters to be sent to function
 * @return Sum of return of all function calls
 */
int list_map(List list, int (*function)(void *, void *), void *params);
/**
 * @fn list_Filter
 * @brief Filters the list, returning a new list with all elements that didn't match the function
 *
 * @param list List to use
 * @param function Function to apply
 * @param params Parameters to be sent to function
 * @return List of all elements removed (this should at least be used to free its memory)
 */
List list_filter(List list, int (*function)(void *, void *), void *params);

void *list_foldr(List list, void *(*func)(void *data, void *recResult, void *params), void *stopCase, void *params);

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
int list_deleteAll(List list, void *params);
void *defDestructor(void *data, void *params);
//</editor-fold>
/**/
//<editor-fold defaultstate="collapsed" desc="DEBUG">
#if _DEBUG == 1
/**
 *
 * @fn list_Print
 * @brief Prints all list data, for debugging
 */
void list_print(List list, void (*printData)(void *));
#endif
//</editor-fold>


#endif
