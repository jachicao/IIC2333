#include "main.h"

bool is_alive(int** array, int m, int n, int i, int j)
{
    return i > -1 && i < m && j > -1 && j < n && array[i][j];
}

int count_alive_around(int** array, int m, int n, int i, int j)
{
    int counter = 0;
    
    int neighbours[8][2] =
    {
        {i - 1	, j - 1	},
        {i			, j - 1	},
        {i + 1	, j - 1	},
        {i + 1	, j			},
        {i + 1	, j + 1	},
        {i			, j + 1	},
        {i - 1	, j + 1	},
        {i - 1	, j			},
    };
    
    for (size_t i = 0; i < 8; i++)
    {
        int* p = neighbours[i];
        int x = p[0];
        int y = p[1];
        if (is_alive(array, m, n, x, y))
        {
            counter++;
        }
    }
    return counter;
}

bool get_new_state(int** array, int m, int n, int i, int j)
{
    int counter = count_alive_around(array, m, n, i, j);
    if (is_alive(array, m, n, i, j))
    {
        if (counter < 2)
        {
            return false;
        }
        else if (counter > 3)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (counter == 3)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

int** two_d_array_init_int(int m, int n)
{
    int** array = (int**) malloc(m * sizeof(int*));
    for(int i = 0; i < m; i++)
    {
        array[i] = (int*) malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
        {
            array[i][j] = 0;
        }
    }
    return array;
}

void two_d_array_destroy_int(int** array, int m, int n)
{
    for(int i = 0; i < m; i++)
    {
        free(array[i]);
    }
    free(array);
}

int** two_d_array_copy_int(int** array, int m, int n)
{
    int** new_array = two_d_array_init_int(m, n);
    for(int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            new_array[i][j] = array[i][j];
        }
    }
    return new_array;
}

int main(int argc, const char * argv[])
{
    if (argc < 2)
    {
        printf("Falta input. Ej: input.txt\n");
        return EXIT_SUCCESS;
    }
    FILE* file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("Failed to read file");
        return EXIT_SUCCESS;
    }
    int t, f, c, v, n;
    fscanf(file, "%d %d %d %d %d", &t, &f, &c, &v, &n);
    int** array = two_d_array_init_int(f, c);
    for (int i = 0; i < v; i++)
    {
        int x;
        int y;
        fscanf(file, "%d %d", &x, &y);
        array[x][y] = 1;
    }
    
    fclose(file);
    
    for (int iteration = 0; iteration < t; iteration++)
    {
        int** temp_array = two_d_array_init_int(f, c);
        for (int i = 0; i < f; i++)
        {
            for (int j = 0; j < c; j++)
            {
                temp_array[i][j] = get_new_state(array, f, c, i, j);
            }
        }
        for (int i = 0; i < f; i++)
        {
            for (int j = 0; j < c; j++)
            {
                array[i][j] = temp_array[i][j];
            }
        }
        two_d_array_destroy_int(temp_array, f, c);
    }
    
    for (int i = 0; i < f; i++)
    {
        for (int j = 0; j < c; j++)
        {
            printf("%d", array[i][j]);
            if (j < c - 1)
            {
                printf(" ");
            }
        }
        printf("\n");
    }
    
    two_d_array_destroy_int(array, f, c);
    return 0;
}
