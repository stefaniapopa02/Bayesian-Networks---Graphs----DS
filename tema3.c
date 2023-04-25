#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tema3.h"
#define max_no_letters 10


int get_idx(char node[10], int number_of_nodes, ATNode *elements){

    int i;
    for(i = 0; i < number_of_nodes; i++){
        if(strcmp(node, elements[i]->name) == 0){
            return i;
        }
    }
}

TGraphL* createGraph(FILE *in) {

	TGraphL *graph = (TGraphL*)calloc(1, sizeof(TGraphL));

    int na; //number of arcs
    int i;
    fscanf(in, "%d", &graph->nn);
    fscanf(in, " %d", &na);

    graph->elements = (ATNode *)calloc(graph->nn, sizeof(ATNode));
    for( i = 0; i < graph->nn; i++){
        graph->elements[i] = (TNode*)calloc(1, sizeof(TNode));
        graph->elements[i]->name = (char*)calloc(max_no_letters, sizeof(char));
        graph->elements[i]->ancestors = (int*)calloc(graph->nn, sizeof(int));
        graph->elements[i]->no_ancestors = 0;

    }

	graph->adl = (ATNode *)calloc(graph->nn, sizeof(ATNode)); 

    for(i = 0; i < graph->nn; i++){
        if(i == 0){
            fscanf(in, "%s", graph->elements[0]->name);
            graph->elements[0]->idx = 0;
        }
        else{
            fscanf(in, " %s", graph->elements[i]->name);
            graph->elements[i]->idx = i;
 
        }
    }

    char node1[10], node2[10];
    int idx1, idx2;

    for(i = 0; i < na; i++){
        fscanf(in, "%s", node1);
        idx1 = get_idx(node1, graph->nn, graph->elements);
        fscanf(in, " %s", node2);
        idx2 = get_idx(node2, graph->nn, graph->elements);

        TNode *new =  (TNode *)malloc(sizeof(TNode));
        new->name = strdup(node2);
        new->idx = idx2;

        new->next = graph->adl[idx1];
        graph->adl[idx1] = new;

    }

	return graph;
}

TGraphL* copyGraph(TGraphL* graph) {

	TGraphL *graph1 = (TGraphL*)calloc(1, sizeof(TGraphL));

    int i;
    graph1->nn = graph->nn;

    graph1->elements = (ATNode *)calloc(graph1->nn, sizeof(ATNode));
    for( i = 0; i < graph1->nn; i++){
        graph1->elements[i] = (TNode*)calloc(1, sizeof(TNode));
        graph1->elements[i]->name = (char*)calloc(max_no_letters, sizeof(char));
        graph1->elements[i]->ancestors = (int*)calloc(graph1->nn, sizeof(int));
        graph1->elements[i]->no_ancestors = 0;

    }

	graph1->adl = (ATNode *)calloc(graph1->nn, sizeof(ATNode)); 
    
    for(i = 0; i < graph1->nn; i++){
        strcpy(graph1->elements[i]->name, graph->elements[i]->name);
        graph1->elements[i]->idx = graph->elements[i]->idx;
    }

    char node1[10], node2[10];
    int idx1, idx2;
    ATNode it;

    for(i = 0; i < graph1->nn; i++){
        for(it = graph->adl[i]; it != NULL; it = it->next ){
            TNode *new =  (TNode *)malloc(sizeof(TNode));
            new->name = strdup(it->name);
            new->idx = it->idx;

            new->next = graph1->adl[i];
            graph1->adl[i] = new;

        }

    }

	return graph1;
}

void freeGraph(TGraphL *graph){
    ATNode it, temp;

    for(int i = 0; i < graph->nn; i++){
        it = graph->adl[i];
        while(it != NULL){
            temp = it;
            it = it->next;
            free(temp->name);
            free(temp);
        }
    }
    free(graph->adl);
    
    for(int i = 0; i < graph->nn; i++){
        free(graph->elements[i]->name);
        free(graph->elements[i]->ancestors);
        free(graph->elements[i]);
    }
    free(graph->elements);
    free(graph);

    return;

}

void dfs_c1(TGraphL* graph, int* visited,int *cycle, int s) {
	
	visited[s] = 1;
    TNode *it;
    int no_anc = graph->elements[s]->no_ancestors;
	for(it = graph->adl[s]; it != NULL; it = it->next ){

        for(int i = 0; i < no_anc; i++){
            if(graph->elements[s]->ancestors[i] == it->idx){
                *cycle = 1;
                return;
            }
        }
        graph->elements[it->idx]->no_ancestors = no_anc + 1;
        for(int i = 0; i < no_anc; i++){
            graph->elements[it->idx]->ancestors[i] = graph->elements[s]->ancestors[i];
        }
        graph->elements[it->idx]->ancestors[no_anc] = s;

        dfs_c1(graph, visited, cycle, it->idx);
        
        if(*cycle == 1){
            break;
        }
	}	
    return;
}


void cerinta1(FILE *in, FILE *out){
    
    TGraphL *graph = createGraph(in);

    int *visited = (int*)calloc(graph->nn, sizeof(int));
    int i, cycle;

    for(i = 0; i < graph->nn; i++){
        cycle = 0;
        if(visited[i] == 0){
            dfs_c1(graph, visited, &cycle, i);
            if(cycle == 1){
                break;
            }
        }
        
    }

    free(visited);

    if(cycle == 1)
    {
        fprintf(out, "imposibil\n");
    }
    else{
        fprintf(out, "corect\n");
    }

    freeGraph(graph);

    return;
}



void dfs_c2(TGraphL* graph, int* visited, int s) { ///we find the ancestors 
	
	visited[s] = 1;
    TNode *it;
	for(it = graph->adl[s]; it != NULL; it = it->next ){

        graph->elements[it->idx]->ancestors[s] = 1;

        for(int i = 0; i < graph->nn; i++){
            if(graph->elements[s]->ancestors[i] == 1){
                graph->elements[it->idx]->ancestors[i] = 1;
            }
        }
        dfs_c2(graph, visited, it->idx);
        
	}	
    return;
}

void remove_idx_i(TGraphL *graph, int i){//We remove the node with index i and the edges that contain this node

    graph->elements[i]->idx = -1;

    int j;
    TNode *it;

    for(j = 0; j < graph->nn; j++){
        if((j != i) && (graph->elements[j]->idx >= 0)){
            for(it = graph->adl[j]; it != NULL; it = it->next ){
                if(it->idx == i){
                    it->idx = -1;
                }
            }
        }
    }
    return;
}

void step1(TGraphL* graph, int x, int y, int n_o, int *o){

    int *visited = (int*)calloc(graph->nn, sizeof(int));
    int i, j;

    for(i = 0; i < graph->nn; i++){
        if(visited[i] == 0){
            dfs_c2(graph, visited, i);   
        }
    }

    int *ancestors = (int*) calloc(graph->nn, sizeof(int));

    ancestors[x] = 1;
    ancestors[y] = 1;

    for(i = 0; i < graph->nn; i++){
        if(graph->elements[x]->ancestors[i] == 1 || graph->elements[y]->ancestors[i] == 1){
            ancestors[i] = 1;
        }
    }

    for(i = 0 ; i < n_o; i++){
        ancestors[o[i]] = 1;
        for(j  = 0; j < graph->nn; j++){
            if(graph->elements[o[i]]->ancestors[j] == 1){
                ancestors[j] = 1;
            }
        }
    }

   for(i = 0; i < graph->nn; i++){
        if(ancestors[i] == 0){
            remove_idx_i(graph, i);
        }
    }
    free(visited);
    free(ancestors);

    return;

}

int exists_node(TGraphL *graph, int i){
    if(graph->elements[i]->idx >= 0)
        return 1;
    return 0;
}

void add_arc(TGraphL *graph, int a, int b){ //add an arc from node 'a' to node 'b'

    TNode *it;

    for(it = graph->adl[a]; it != NULL; it = it->next){
        if(it->idx == b)
            return;
    }

    TNode *new =  (TNode *)malloc(sizeof(TNode));
    new->name = strdup(graph->elements[b]->name);
    new->idx = b;

    new->next = graph->adl[a];
    graph->adl[a] = new;

    return;

}


void step2(TGraphL *graph){
    for(int j = 0; j < graph->nn; j++){
        if(exists_node(graph, j)){
             for(int l = 0; l < graph->nn; l++){
                graph->elements[j]->ancestors[l] = 0;//in this step we use the "ancestors" field as an array of parents
            }
            graph->elements[j]->no_ancestors = 0;
        }
    }

    int i, j, l;

    for(i = 0; i < graph->nn; i++){
        if(exists_node(graph, i)){
            for(j = 0; j < graph->nn; j++){
                if(exists_node(graph, j)){
                    TNode *it;
                    for(it = graph->adl[j]; it != NULL; it = it->next){
                        if(it->idx == i){
                            int no_ancestors = graph->elements[i]->no_ancestors;
                            graph->elements[i]->ancestors[no_ancestors] = j;
                            graph->elements[i]->no_ancestors++;
                        }
                    }
                }
            }
        }
    }
   

    for(i = 0; i < graph->nn; i++){
        int no_anc = graph->elements[i]->no_ancestors;
        if(exists_node(graph, i) && (no_anc >= 2)){
            for(j = 0; j < no_anc - 1; j++){
                for( l = j + 1; l < no_anc; l++){
                    add_arc(graph, graph->elements[i]->ancestors[j], graph->elements[i]->ancestors[l]);
                    add_arc(graph, graph->elements[i]->ancestors[l], graph->elements[i]->ancestors[j]);
                    
                }
            }
        }
    }

    return;
}

void step3(TGraphL *graph){
    int i, j;
    for(int i = 0; i < graph->nn; i++){
        if(exists_node(graph, i)){
            ATNode it;
            for(it = graph->adl[i]; it != NULL; it = it->next){
                if(it->idx >= 0){
                    add_arc(graph, it->idx, i);
                }
            }
        }
    }

    return;
}


void step4(TGraphL *graph, int n_o, int *o){
    
    for(int i = 0; i < n_o; i++){
        remove_idx_i(graph, o[i]);
    }

    return;
}

void dfs(TGraphL *graph, int *visited, int x, int node){

    visited[node] = 1;
    ATNode it;
    for(it = graph->adl[node]; it != NULL; it = it->next){
        if(it->idx >= 0){
            if(visited[it->idx] == 0){
                int no_ancestors = graph->elements[x]->no_ancestors;
                graph->elements[x]->ancestors[no_ancestors] = it->idx;
                graph->elements[x]->no_ancestors++;
                dfs(graph, visited, x, it->idx);
            }
        }
    }
    return;
}

void step5(FILE *out, TGraphL *graph, int x, int y){

    if(!exists_node(graph, x)){
        fprintf(out, "independente\n");
        return;
    }
    if(!exists_node(graph, y)){
        fprintf(out, "independente\n");
        return;
    }

    graph->elements[x]->no_ancestors = 0;//we'll use the 'ancestor' field as a path array

    int *visited = (int*)calloc(graph->nn, sizeof(int));

    dfs(graph, visited, x, x);

    int search_for_y = 0;

    for(int i = 0; i < graph->elements[x]->no_ancestors; i++){
        if(graph->elements[x]->ancestors[i] == y){
            search_for_y = 1;
            break;
        }
    }

    if(search_for_y == 0){
        fprintf(out, "independente\n");
    }
    else
    {
        fprintf(out, "neindependente\n");
    }

    free(visited);

    return;
}

void cerinta2(FILE *in, FILE *out){

    TGraphL *graph = createGraph(in);
    TGraphL *graph1;
    int k; // number of queries

    fscanf(in, "%d\n", &k);

    char *X, *Y;
    char *word;
    int x, y;
    char *O ;
    char *line= (char*) calloc(100, sizeof(char));
    int n_o, *o;
    o = (int*) calloc(11, sizeof(int));

    for(int i = 1; i <= k; i++){

        graph1 = copyGraph(graph);
       
        fgets(line, 100, in);

        X = strtok(line, ";| \n");
        x = get_idx(X, graph1->nn, graph1->elements);

        Y = strtok(NULL, ";| \n");
        y = get_idx(Y, graph1->nn, graph1->elements);

        O = strtok(NULL, ";| \n");
        n_o = 0;

        while(O != NULL){
            o[n_o] = get_idx(O, graph1->nn, graph1->elements);
            n_o++;
            O = strtok(NULL, ";| \n ");
        }

        for(int j = 0; j < graph1->nn; j++){
            graph1->elements[j]->no_ancestors = graph1->nn;
            for(int l = 0; l < graph1->nn; l++){
                graph1->elements[j]->ancestors[l] = 0;
            }
        }

        step1(graph1, x, y, n_o, o);
        step2(graph1);
        step3(graph1);
        step4(graph1, n_o, o);
        step5(out, graph1, x, y);

        freeGraph(graph1);

    }

      
        free(line);
        free(o);

        freeGraph(graph);
    return;

}

int main(int argc, char *argv[])
{
    FILE *in = fopen("bnet.in", "r");
    FILE *out = fopen("bnet.out", "w");
    if(strcmp(argv[1], "-c1") == 0){
        cerinta1(in, out);
    }
    else if(strcmp(argv[1], "-c2") == 0){
        cerinta2(in, out);
    }
    fclose(in);
    fclose(out);
    
}