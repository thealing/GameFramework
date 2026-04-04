#pragma once

#include <stdlib.h>

typedef struct List List;

typedef struct List_Node List_Node;

struct List
{
	List_Node* first;
	
	List_Node* last;

	int size;
};

struct List_Node
{
	List_Node* prev;
	
	List_Node* next;

	List* list;

	void* item;
};

void list_insert_first(List* list, List_Node* node_to_insert);

void list_insert_last(List* list, List_Node* node_to_insert);

List_Node* list_insert_first_item(List* list, const void* item_to_insert);

List_Node* list_insert_last_item(List* list, const void* item_to_insert);

List_Node* list_node_create(const void* item);

void list_node_destroy(List_Node* node);

void list_node_insert_next(List_Node* node, List_Node* node_to_insert);

void list_node_insert_prev(List_Node* node, List_Node* node_to_insert);

List_Node* list_node_insert_next_item(List_Node* node, const void* item_to_insert);

List_Node* list_node_insert_prev_item(List_Node* node, const void* item_to_insert);

void list_node_swap_with_next(List_Node* node);

void list_node_swap_with_prev(List_Node* node);

void list_node_remove(List_Node* node);
