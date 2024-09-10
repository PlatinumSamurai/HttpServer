#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>


typedef struct Node {
	char *key;
	char *value;
} Node;


typedef struct List {
	Node *node;
	struct List *next;
} List;


List* ListInit(Node *);
void ListInsert(List *, Node *);
int ListFree(List *);

#endif  // LIST_H_
