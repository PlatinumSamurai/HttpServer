#include "list.h"


List* ListInit(Node *node) {
	List *head = (List *)malloc(sizeof(List));

	head->node = node;
	head->next = NULL;
    
    return head;
}


void ListInsert(List *list, Node *node) {
	List *tmp = list;
	
	while (tmp->next) {
		tmp = tmp->next;
	}
	tmp->next = (List *)malloc(sizeof(List));
	tmp->next->node = node;
	tmp->next->next = NULL;
	
}

int ListFree(List *list) {
	List *tmp = list->next;
	
	while (list) {
		free(list->node->key);
		free(list->node->value);
		free(list->node);
		free(list);
		list = tmp;
		tmp = tmp ? tmp->next : NULL;
	}
	
    return 0;
}
