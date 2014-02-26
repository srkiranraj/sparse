/*---------------------------------------
// Utility function to check if id is
// present in 2 different arrays.
---------------------------------------*/
int alreadyAdded(int id, int *part1, int part1_limit, int *part2, int part2_limit)
{
    /*---------------------------------------
    // Return Values Description
    // 1 -> Already Added
    // 0 -> Not yet Added
    ---------------------------------------*/

    int i;

    for (i = 0; i < part1_limit; ++i)
    {
        if(part1[i] == id)
            return 1;
    }

    for (i = 0; i < part2_limit; ++i)
    {
        if(part2[i] == id)
            return 1;
    }

    return 0;
}
