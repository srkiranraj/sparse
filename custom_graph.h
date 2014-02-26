/*---------------------------------------
// Structure to represent node of an 
// adjacency list.
---------------------------------------*/
struct AdjListNode
{
    int dest;
    struct AdjListNode* next;
};
 
/*---------------------------------------
// Structure to represent an adjacency
// list.
---------------------------------------*/
struct AdjList
{
    struct AdjListNode *head;  // Pointer to head node of list
};
 
/*---------------------------------------
// Structure to represent a graph.
// Graph is an array of adjacency lists.
// Size of array will be V.
// Where V is number of vertices in graph.
---------------------------------------*/
struct Graph
{
    int V;
    struct AdjList* array;
};

/*---------------------------------------
// Utility function to create a new node
// in adjacency list.
---------------------------------------*/
struct AdjListNode* newAdjListNode(int dest)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}
 
/*---------------------------------------
// Utility function that creates a graph
// of V vertices.
---------------------------------------*/
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
 
    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
 
     // Initialize each adjacency list as empty by making head as NULL
    int i;
    for (i = 0; i < V; ++i)
        graph->array[i].head = NULL;
 
    return graph;
}

 
/*---------------------------------------
// Utility function that adds an edge
// to a (un)directed graph.
---------------------------------------*/
void addEdge(struct Graph* graph, int src, int dest)
{
    /*---------------------------------------
    // Add an edge from src to dest.
    // A new node is added to the adjacency
    // list of src.
    // The node is added at the begining.
    ---------------------------------------*/

    struct AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
}

/*---------------------------------------
// Utility function to check whether edge
// src -> dest is present in given graph
// or not.
---------------------------------------*/
int isEdgePresent(struct Graph* graph, int src, int dest)
{
    struct AdjListNode* pCrawl = graph->array[src].head;
    while(pCrawl)
    {
        if(pCrawl->dest == dest)
            return 1;
        pCrawl = pCrawl->next;
    }   
    return 0;
}

/*---------------------------------------
// Utility function that subtracts two
// graphs G1, G2 and creates a new graph G.
---------------------------------------*/
void subtract_graphs(struct Graph* G, struct Graph* G1, struct Graph* G2)
{
    int v;

    for (v = 0; v < G1->V; ++v)
    {
        struct AdjListNode* pCrawl = G1->array[v].head;
        while (pCrawl)
        {
            if(!isEdgePresent(G2, v, pCrawl->dest))
                addEdge(G, v, pCrawl->dest);
            pCrawl = pCrawl->next;
        }
    }   
}

/*---------------------------------------
// Utility function that constructs a
// bi-partite subgraph BG of the given 
// graph G.
---------------------------------------*/
void compute_max_bp_subgraph(struct Graph* BG, struct Graph* G, int *edgeLoss, int debug)
{
    int V, i;
    int track1, track2;
    int count1, count2;
    int *part1, *part2;
    struct AdjListNode *tempNode;
    struct AdjListNode *tempNode1;

    char *newline = "\n";

    V = G->V;

    part1 = (int *)malloc(V * sizeof(int));
    part2 = (int *)malloc(V * sizeof(int));
    for (i = 0; i < V; ++i)
    {
        part1[i] = -1;
        part2[i] = -1;
    }

    /*---------------------------------------
    // Step III (a) :: Add vertex(0) to part1
    // and its neighbours to part2
    ---------------------------------------*/
    part1[0] = 0;
    track1 = 1;
    track2 = 0;

    /*---------------------------------------
    // Commented as every node should have the
    // freedom to choose their partitions
    ---------------------------------------*/        
    // tempNode = G1->array[part1[0]].head;
    // while(tempNode)
    // {
    //     part2[track2] = tempNode->dest;
    //     track2++;
    //     tempNode = tempNode->next;
    // }

    /*---------------------------------------
    // Step III (b) :: Add rest of the vertices
    // to part1 or  part2 based on number of
    // neighbours in each part
    ---------------------------------------*/

    for (i = 1; i < V; ++i)
    {
        count1 = 0;
        count2 = 0;
        if(!alreadyAdded(i, part1, track1, part2, track2))
        {
            tempNode = G->array[i].head;
            if(tempNode)
            {
                while(tempNode)
                {
                    if(alreadyAdded(tempNode->dest, part1, track1, part2, 0))
                        count1++;
                    else if(alreadyAdded(tempNode->dest, part1, 0, part2, track2))
                        count2++;
                    tempNode = tempNode->next;
                }
                if(count1 < count2)
                {
                    part1[track1] = i;
                    track1++;
                }
                else
                {
                    part2[track2] = i;
                    track2++;
                }
            }
        }
    }

    if(debug)
    {
        print_1darray(part1, 0, track1, "Partition 1", newline);
        print_1darray(part2, 0, track2, "Partition 2", newline);
    }

    /*---------------------------------------
    // Step III (c) :: Generate Graph G2 with
    // same number of vertices but less edges
    // according to bi-partite partition.
    ---------------------------------------*/
    for(i = 0; i < V; i++)
    {
        if(alreadyAdded(i, part1, track1, part2, 0))
        {
            tempNode = G->array[i].head;
            if(tempNode)
            {
                while(tempNode)
                {
                    if(alreadyAdded(tempNode->dest, part1, 0, part2, track2))
                        addEdge(BG, i, tempNode->dest);
                    else
                        edgeLoss[i] = edgeLoss[i] + 1;
                    tempNode = tempNode->next;
                }
            }
        }
        else
        {
            tempNode = G->array[i].head;
            if(tempNode)
            {
                while(tempNode)
                {
                    if(alreadyAdded(tempNode->dest, part1, track1, part2, 0))
                        addEdge(BG, i, tempNode->dest);
                    else
                        edgeLoss[i] = edgeLoss[i] + 1;
                    tempNode = tempNode->next;
                }   
            }
        }
    }
}

/*---------------------------------------
// Utility function to use the adjacency
// list representation of graph and 
// calculate indegree of each node.
---------------------------------------*/
void calcdegree(struct Graph* graph, int *degree)
{
    int v;

    for (v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* pCrawl = graph->array[v].head;
        while (pCrawl)
        {
            degree[v] = degree[v] + 1;
            pCrawl = pCrawl->next;
        }
    }
}
 
/*---------------------------------------
// Utility function to print the adjacency
// list representation of graph.
---------------------------------------*/
void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* pCrawl = graph->array[v].head;
        printf("Adjacency list of vertex %d => head ", v);
        while (pCrawl)
        {
            printf("-> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
    printf("\n");
}

/*---------------------------------------
// Utility function to count edges of
// graph.
---------------------------------------*/
int countEdges(struct Graph* graph)
{
    int v;
    int count = 0;
    for (v = 0; v < graph->V; ++v)
    {
        struct AdjListNode* pCrawl = graph->array[v].head;
        while (pCrawl)
        {
            count++;
            pCrawl = pCrawl->next;
        }
    }
    return count;
}

/*---------------------------------------
// Utility function to calculate volume
// of a given graph.
---------------------------------------*/
void calculate_volume(struct Graph* graph, int **A, int rows)
{
    int i = 0;

    struct AdjListNode *tempNode;
    struct AdjListNode *tempNode1;

    for (i = 0; i < rows; ++i)
    {
        tempNode = graph->array[i].head;
        while(tempNode)
        {
            tempNode1 = graph->array[tempNode->dest].head;
            while(tempNode1)
            {
                A[i][tempNode1->dest] = A[i][tempNode1->dest] + 1;
                tempNode1 = tempNode1->next;
            }
            tempNode = tempNode->next;
        }
    }
}

/*---------------------------------------
// Utility function to calculate approx.
// volume of a given graph. Considering
// edge loss due to,
// I. Removal of vertex with low degree
// II. Greedy bi-partite construction
---------------------------------------*/
int* print_approx(int **A2, int rows, int cols, int withRowSum, int onlyRowSum, char *title, int actual_rows, int actual_cols, int k, int *degree, int *edgeLoss1, int *edgeLoss2, char *space)
{
    int i, j;
    int rowSum = 0;

    int *rs;
    rs = (int *)malloc(actual_rows * sizeof(int));

    int average_degree;
    int sum = 0;
    for (i = 0; i < actual_rows; ++i)
        sum = sum + degree[i];
    average_degree = sum / actual_rows;

    printf("\n%s\n", title);
    for (i = 0; i < actual_rows; ++i)
    {
        for (j = 0; j < actual_cols; ++j)
        {
            rowSum = rowSum + A2[i][j];
            if(!onlyRowSum)
                printf("%d%s", A2[i][j], space);
        }
        if(withRowSum)
        {
            rowSum = rowSum + edgeLoss2[i] * average_degree + edgeLoss1[i] * k;
            printf("=> %d", rowSum);
        }
        rs[i] = rowSum;
        rowSum = 0;
        if(!onlyRowSum)
            printf("\n");
    }
    printf("\n");

    printf("\nAverage Degree :: %d\n\n", average_degree);

    return rs;    
}
