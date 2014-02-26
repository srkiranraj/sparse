/*---------------------------------------
// Header File(s)
---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "custom_utility.h"
#include "custom_print.h"
#include "custom_graph.h"
 
/*---------------------------------------
// Main :: Execution Begins Here
---------------------------------------*/
int main()
{

    /*---------------------------------------
    // Configurable Parameters
    ---------------------------------------*/    
    int debug = 1;  // 1 => Debug Mode, 0 => Production Mode
    int isBig = 0;  // 1 => Input is Big, 0 => Small
    int k = 3;      // Minimum number of edges for each vertices in G1

    /*---------------------------------------
    // Helper Parameters
    ---------------------------------------*/
    int withRowSum = 1;
    int withOutRowSum = 0;
    char *space = " ";
    char *newline = "\n";
    char *tabspace = "\t";

    int loop = 0;
    int i = 0;
    int j = 0;
    int dummy, temp = 0;
    int src, dest;

    /*---------------------------------------
    // Problem Specific Parameters
    ---------------------------------------*/
    FILE *fp;       // File Pointer to read files
    int rows;       // Number of rows in sparse matrix
    int cols;       // Number of columns in sparse matrix
    int nz;         // Number of non-zero entries in sparse matrix
    
    struct Graph *G;
    struct Graph *G1;
    struct Graph *G2;
    struct Graph *SG;
    struct Graph *MSG;

    int V, V1, V2;
    int **A, **A1, **A2, **SA;

    int *degree;
    int *edgeLoss1, *edgeLoss2;
    int *dummyEdgeLoss;

    int *part1, *part2;
    int track1, track2;
    int count1, count2;

    struct AdjListNode *tempNode;
    struct AdjListNode *tempNode1;


    printf("\n");

    /*---------------------------------------
    // Step I :: Create the graph - G
    // By reading the given sparse matrix A
    ---------------------------------------*/
    {
        fp = fopen("./input/Stranke94_Symmetric.txt", "r");
        if(fp == NULL)
        {
            perror("Error while opening the file.\n");
            exit(EXIT_FAILURE);
        }

        fscanf(fp, "%d", &rows);
        fscanf(fp, "%d", &cols);
        fscanf(fp, "%d", &nz);

        V = rows;
        G = createGraph(V);

        A = (int **)malloc(rows * sizeof(int *));
        for (i = 0; i < rows; i++)
            A[i] = (int *)calloc(cols, sizeof(int));
        
        for (loop = 0; loop < nz; ++loop)
        {
            fscanf(fp, "%d", &src);
            fscanf(fp, "%d", &dest);
            fscanf(fp, "%d", &dummy);
            
            addEdge(G, src-1, dest-1);
            addEdge(G, dest-1, src-1);
            
            A[src-1][dest-1] = dummy;
            A[dest-1][src-1] = dummy;
        }


        printf("Graph G has %d vertices and %d edges.\n", V, countEdges(G));        
        if(debug)
        {
            print_2darray(A, rows, cols, withOutRowSum, isBig, "Given Sparse Matrix A", tabspace);
            printGraph(G);
        }

    }

    /*---------------------------------------
    // Step II :: Obtain reduced graph - G1
    // By eliminating nodes with degree < k
    ---------------------------------------*/
    {
        degree = (int *)calloc(V, sizeof(int));
        edgeLoss1 = (int *)calloc(V, sizeof(int));
            
        calcdegree(G, degree);

        V1 = 0;

        for (i = 0; i < V; ++i)
            if(degree[i] >= k)
                V1++;

        G1 = createGraph(V);

        for (i = 0; i < V; ++i)
        {
            if(degree[i] >= k)
            {
                tempNode = G->array[i].head;
                while (tempNode)
                {
                    if(degree[tempNode->dest] >= k)
                        addEdge(G1, i, tempNode->dest);
                    else
                        edgeLoss1[i] = edgeLoss1[i] + 1;
                    tempNode = tempNode->next;
                }
            }
            else
            {
                tempNode = G->array[i].head;
                while (tempNode)
                {
                    edgeLoss1[i] = edgeLoss1[i] + 1;
                    tempNode = tempNode->next;
                }
            }
        }

        printf("Graph G1 has %d vertices and %d edges.\n", V1, countEdges(G1));
        if(debug)
        {
            print_1darray(degree, 0, V, "Degree of all vertices", newline);  
            print_1darray(edgeLoss1, 0, V, "Edge Loss due to removal of small degree vertices", newline);  
            printGraph(G1);
        }
    }

    /*---------------------------------------
    // Step III :: Obtain reduced graph - G2
    // By finding a large bipartite graph of G1
    ---------------------------------------*/
    {
        edgeLoss2 = (int *)calloc(V, sizeof(int));
        V2 = V1;
        G2 = createGraph(V);

        compute_max_bp_subgraph(G2, G1, edgeLoss2, debug);

        printf("Graph G2 has %d vertices and %d edges.\n", V2, countEdges(G2));
        if(debug)
        {
            print_1darray(edgeLoss2, 0, V, "Edge loss due to bi-partite construction", newline);
            printGraph(G2);
        }
    }

    /*---------------------------------------
    // Step IV :: Calculate Volume for A2 * A2
    ---------------------------------------*/
    {
        for (i = 0; i < V; ++i)
            for (j = 0; j < V; ++j)
                A[i][j] = 0;

        calculate_volume(G, A, V);

        A2 = (int **)malloc(V * sizeof(int *));

        for (i = 0; i < V; i++)
            A2[i] = (int *)calloc(V, sizeof(int));

        calculate_volume(G2, A2, V);

        int *try1 = print_2darray(A, rows, cols, withRowSum, isBig, "Volume of G i.e A * A", tabspace);
        print_2darray(A2, V, V, withRowSum, isBig, "Volume for G2 i.e A2 * A2", tabspace);
        int *try2 = print_approx(A2, V, V, withRowSum, isBig, "Volume of G i.e A * A (Approx)", V, V, k, degree, edgeLoss1, edgeLoss2, tabspace);
        print_to_file(try1, try2, V, "Load Comparison: Original Sparse Matrix Vs. Approximated Sparse Matrix");
    }

    /*---------------------------------------
    // Step V :: Calculate volume of log n 
    // sub-graphs obtained from actual graph.
    // G (log n) = G - G2 - G3 - ... G (log n - 1)
    // Add up all these volumes for effective
    // approximated volume.
    ---------------------------------------*/
    {
        dummyEdgeLoss = (int *)calloc(V, sizeof(int));
        SA = (int **)malloc(V * sizeof(int *));

        for (i = 0; i < V; i++)
            SA[i] = (int *)calloc(V, sizeof(int));

        SG = createGraph(V);
        subtract_graphs(SG, G, G2);
        printGraph(SG);

        MSG = createGraph(V);
        compute_max_bp_subgraph(MSG, SG, dummyEdgeLoss, debug);
        printGraph(MSG);

        calculate_volume(MSG, A2, V);
        print_2darray(A2, V, V, withRowSum, isBig, "Volume for G3 i.e (G - G3) * (G - G3)", tabspace);

        print_approx(A2, V, V, withRowSum, isBig, "Volume of G i.e A * A (Approx) after I iteration", V, V, k, degree, edgeLoss1, edgeLoss2, tabspace);
    }
 
    return 0;
}
