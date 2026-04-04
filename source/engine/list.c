#include "list.h"

static Pool* s_list_pool;

static Pool** s_list_pool_stack[LIST_POOL_STACK_SIZE];

static size_t s_list_pool_stack_index;

void list_set_pool(Pool* pool)
{
	if (pool != NULL)
	{
		s_list_pool_stack[s_list_pool_stack_index] = s_list_pool;

		s_list_pool_stack_index++;

		s_list_pool = pool;
	}
	else
	{
		s_list_pool_stack_index--;

		s_list_pool = s_list_pool_stack[s_list_pool_stack_index];
	}
}

void list_insert_first(List* list, List_Node* node_to_insert)
{
	if (list->size == 0)
	{
		node_to_insert->list = list;
		
		list->first = list->last = node_to_insert;
		
		list->size = 1;
	}
	else
	{
		list_node_insert_prev(list->first, node_to_insert);
	}
}

void list_insert_last(List* list, List_Node* node_to_insert)
{
	if (list->size == 0)
	{
		node_to_insert->list = list;
		
		list->first = list->last = node_to_insert;
		
		list->size = 1;
	}
	else
	{
		list_node_insert_next(list->last, node_to_insert);
	}
}

List_Node* list_insert_first_item(List* list, const void* item_to_insert)
{
	List_Node* node_to_insert = list_node_create(item_to_insert);
	
	list_insert_first(list, node_to_insert);
	
	return node_to_insert;
}

List_Node* list_insert_last_item(List* list, const void* item_to_insert)
{
	List_Node* node_to_insert = list_node_create(item_to_insert);
	
	list_insert_last(list, node_to_insert);
	
	return node_to_insert;
}

List_Node* list_node_create(const void* item)
{
	List_Node* node;

	if (s_list_pool != NULL)
	{
		node = pool_alloc(s_list_pool);
	}
	else
	{
		node = calloc(1, sizeof(List_Node));
	}
	
	node->item = (void*)item;
	
	return node;
}

void list_node_destroy(List_Node* node)
{
	if (node->list != NULL)
	{
		list_node_remove(node);
	}

	if (s_list_pool != NULL)
	{
		pool_free(s_list_pool, node);
	}
	else
	{
		free(node);
	}
}

void list_node_insert_next(List_Node* node, List_Node* node_to_insert)
{
	node_to_insert->prev = node;
	
	node_to_insert->next = node->next;
	
	node_to_insert->list = node->list;

	if (node == node->list->last)
	{
		node->list->last = node_to_insert;
	}
	else
	{
		node->next->prev = node_to_insert;
	}

	node->next = node_to_insert;

	node->list->size++;
}

void list_node_insert_prev(List_Node* node, List_Node* node_to_insert)
{
	node_to_insert->next = node;
	
	node_to_insert->prev = node->prev;
	
	node_to_insert->list = node->list;

	if (node == node->list->first)
	{
		node->list->first = node_to_insert;
	}
	else
	{
		node->prev->next = node_to_insert;
	}

	node->prev = node_to_insert;

	node->list->size++;
}

List_Node* list_node_insert_next_item(List_Node* node, const void* item_to_insert)
{
	List_Node* node_to_insert = list_node_create(item_to_insert);
	
	list_node_insert_next(node, node_to_insert);
	
	return node_to_insert;
}

List_Node* list_node_insert_prev_item(List_Node* node, const void* item_to_insert)
{
	List_Node* node_to_insert = list_node_create(item_to_insert);
	
	list_node_insert_prev(node, node_to_insert);
	
	return node_to_insert;
}

void list_node_swap_with_next(List_Node* node)
{
	node->next->prev = node->prev;

	if (node->prev != NULL)
	{
		node->prev->next = node->next;
	}

	node->prev = node->next;
	
	node->next = node->next->next;
	
	node->prev->next = node;

	if (node->next != NULL)
	{
		node->next->prev = node;
	}

	if (node->prev == node->list->last)
	{
		node->list->last = node;
	}

	if (node == node->list->first)
	{
		node->list->first = node->prev;
	}
}

void list_node_swap_with_prev(List_Node* node)
{
	node->prev->next = node->next;

	if (node->next != NULL)
	{
		node->next->prev = node->prev;
	}

	node->next = node->prev;
	
	node->prev = node->prev->prev;
	
	node->next->prev = node;

	if (node->prev != NULL)
	{
		node->prev->next = node;
	}

	if (node->next == node->list->first)
	{
		node->list->first = node;
	}

	if (node == node->list->last)
	{
		node->list->last = node->next;
	}
}

void list_node_remove(List_Node* node)
{
	if (node == node->list->last)
	{
		node->list->last = node->prev;
	}
	else
	{
		node->next->prev = node->prev;
	}

	if (node == node->list->first)
	{
		node->list->first = node->next;
	}
	else
	{
		node->prev->next = node->next;
	}

	node->list->size--;
	
	node->list = NULL;
}
