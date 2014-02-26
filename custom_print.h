/*---------------------------------------
// Utility function to print one
// dimensional array.
---------------------------------------*/
void print_1darray(int *array, int start, int end, char *title, char *space)
{
    int ptr = 0;

    printf("\n%s\n", title);
    for(ptr = start; ptr < end; ++ptr)
        printf("%d%s", array[ptr], space);
    printf("\n\n");

    return;
}

/*---------------------------------------
// Utility function to print two
// dimensional array.
---------------------------------------*/
int* print_2darray(int **A, int rows, int cols, int withRowSum, int onlyRowSum, char *title, char *space)
{
    int i, j;
    int rowSum = 0;

    int *rs;
    rs = (int *)malloc(rows * sizeof(int));

    printf("\n%s\n", title);
    for (i = 0; i < rows; ++i)
    {
        for (j = 0; j < cols; ++j)
        {
            rowSum = rowSum + A[i][j];
            if(!onlyRowSum)
                printf("%d%s", A[i][j], space);
        }
        if(withRowSum)
            printf("=> %d ", rowSum);
        rs[i] = rowSum;
        rowSum = 0;
        if(!onlyRowSum)
            printf("\n");
    }
    printf("\n");

    return rs;
}

/*---------------------------------------
// Utility function to print two
// one dimensional array in parallel.
---------------------------------------*/
void print_to_file(int *try1, int *try2, int V, char *title)
{

    FILE *fw = fopen("load_output.txt", "w");
    if (fw == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    printf("\n%s\n", title);
    for (int i = 0; i < V; ++i)
    {
        fprintf(fw, "%5d\t-\t%d\n", try1[i], try2[i]);
        printf("%d\t%d\n", try1[i], try2[i]);
    }
    printf("\n");
    fclose(fw);
}
