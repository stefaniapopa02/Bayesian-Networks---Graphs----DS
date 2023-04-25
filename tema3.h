typedef struct Node{
	int idx;
    char *name;
    struct Node *next; 
	int *ancestors;
	int no_ancestors;
	
}TNode, *ATNode;

typedef struct{
	int nn; 
	ATNode *elements;
	ATNode *adl;  
}TGraphL;