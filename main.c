#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int first;
    int second;
    struct node *next;
} Node;

typedef struct graph {
    int nodes_num;
    int** matrix;
} Graph;

Node* new_node(int first, int second) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node -> first = first;
    new_node -> second = second;
    new_node -> next = NULL;
    return new_node;
}

Node* add_node(Node* cur_node, int first, int second) { // adds new node to the list
    Node *new_node_t = new_node(first, second);
    new_node_t -> next = cur_node;
    return new_node_t;
}

void del_nodes(Node* cur_node) { //delete nodes
    if (cur_node == NULL)
        return;
    del_nodes(cur_node -> next);
    free(cur_node);
}

void print_rec(Node* cur_node) { // prints list of edges to console
    if (cur_node == NULL)
        return;
    print_rec(cur_node->next);
    printf("%d -- %d\n", cur_node -> first, cur_node -> second);
}

void print_to_file_rec(Node* cur_node, FILE* file) { //prints list of edges to graphvis file
    if (cur_node == NULL)
        return;
    print_to_file_rec(cur_node -> next, file);
    fprintf(file, "%d -- %d\n", cur_node -> first, cur_node -> second);
}

void make_graph_file(Node* list) { //writes file for graphvis
    FILE *dot_file = fopen("..\\forgraph.dot", "w");
    fputs("graph {\n", dot_file);
    print_to_file_rec(list, dot_file);
    fputs("}", dot_file);
    fclose(dot_file);
}

Node *make_list(FILE* Node_file) {
    int from, to;
    Node *list = NULL;
    while(fscanf(Node_file, "%d %d", &from, &to) != EOF){
        list = add_node(list, from, to); //scan edges from file to the singly linked list
    }
    return list;
}

int count_nodes(Node* list){
    int num_of_nodes = 0;
    while (list != NULL) {
        if (list -> first > num_of_nodes)
            num_of_nodes = list -> first;
        if (list -> second > num_of_nodes)
            num_of_nodes = list -> second;
        list = list -> next;
    }
    return num_of_nodes;
}

Graph* initialize_matrix(int num_nodes) {
    Graph *new_graph = malloc(num_nodes * sizeof(Node));
    new_graph -> matrix = (int**)malloc(num_nodes * sizeof(int*));
    for (int i = 0; i < num_nodes; i++) {
        new_graph -> matrix[i] = (int*)calloc( num_nodes, sizeof(int));
    }
    new_graph -> nodes_num = num_nodes;
    return new_graph;
}

void create_matrix(Graph* graph, Node* list) {
    while (list != NULL) {
        int first = list -> first;
        int second = list -> second;
        graph -> matrix[first - 1][second - 1] = 1;
        list = list -> next;
    }
}

void graphs_intersection(Graph* graph1, Graph* graph2){
    FILE* result_file = fopen("..\\result_graph.txt", "w");
    if (graph1 -> nodes_num > graph2 -> nodes_num){
        Graph* buf = graph1;
        graph1 = graph2;
        graph2 = buf;
    }

    for (int i = 0; i < graph1 -> nodes_num; i++){
        for (int j = 0; j < graph1 -> nodes_num; j++){
            if (graph1 -> matrix[i][j] == 1 && graph2 -> matrix[i][j] == 1){
                fprintf(result_file, "%d %d\n", i + 1, j + 1);
            }
        }
    }
    fclose(result_file);
}

int main() {

    FILE* first_graph_file = fopen("..\\first_graph.txt", "r"); //open start file with list of edges
    if (first_graph_file == NULL) {
        puts("Error with opening file first_graph.txt");
        exit(-1);
    }

    Graph* graph1;
    Node* first_list = make_list(first_graph_file);
    fclose(first_graph_file);
    int nodes_num = count_nodes(first_list);
    graph1 = initialize_matrix(nodes_num);
    create_matrix(graph1, first_list);

    FILE* second_graph_file = fopen("..\\second_graph.txt", "r");
    if (second_graph_file == NULL) {
        puts("Error with opening file second_graph.txt");
        exit(-1);
    }

    Graph* graph2 = NULL;
    Node* second_list = make_list(second_graph_file);
    fclose(second_graph_file);
    int nodes_num2 = count_nodes(second_list);
    graph2 = initialize_matrix(nodes_num2);
    create_matrix(graph2, second_list);

    FILE* result_graph_file = fopen("..\\result_graph.txt", "r");
    if (result_graph_file == NULL) {
        puts("Error with opening file result_graph.txt");
        exit(-1);
    }

    Node*  result_list = make_list(result_graph_file);
    fclose(result_graph_file);
    puts("List of edges:");
    print_rec(result_list);


    graphs_intersection(graph1, graph2);
    make_graph_file(result_list);


    system("dot -Tpng -O ..\\forgraph.dot");

    del_nodes(first_list);
    del_nodes(second_list);
    del_nodes(result_list);

    return 0;
}
