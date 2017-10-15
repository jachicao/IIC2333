#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Enums.h"

void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
    int L[n1], R[n2];
    
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
    
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(int* arr, int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        
        merge(arr, l, m, r);
    }
}

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        printf("falta politica\n");
        return EXIT_FAILURE;
    }
    enum scheduling_type type;
    if (strcmp(argv[1], "fcfs") == 0) {
        type = FCFS;
    } else if (strcmp(argv[1], "sstf") == 0) {
        type = SSTF;
    } else if (strcmp(argv[1], "scan") == 0) {
        type = SCAN;
    } else if (strcmp(argv[1], "c-look") == 0) {
        type = CLOOK;
    } else {
        printf("politica desconocida\n");
        return EXIT_FAILURE;
    }
    if (argc < 3) {
        printf("falta input\n");
        return EXIT_FAILURE;
    }
    FILE* file = fopen(argv[2], "r");
    if (file != NULL) {
        int* array = (int*) malloc(sizeof(int));
        int length = 0;
        bool is_head = true;
        int head = 0;
        int head_index = 0;
        int number;
        while(fscanf(file, "%d", &number) == 1) {
            if (is_head) {
                head = number;
                is_head = false;
            }
            array = (int*) realloc(array, (length + 1) * sizeof(int));
            array[length] = number;
            length++;
        }
        /*
        merge_sort(array, 0, length - 1);
        for (int i = 0; i < length; i++) {
            if (array[i] == head) {
                head_index = i;
                break;
            }
        }
         */
        int sum = 0;
        int x = 0;
        int y = 0;
        int left = length;
        int current_index = head_index;
        int* order_array = (int*) malloc(length);
        int order_array_length = 0;
        switch (type) {
            case FCFS:
                for (int i = 1; i < length; i++) {
                    order_array[order_array_length] = array[i];
                    order_array_length++;
                }
                for (int i = 1; i < length; i++) {
                    sum += abs(array[i] - array[i - 1]);
                }
                break;
            case SSTF:
                while (left > 0) {
                    int min_diff = -1;
                    int min_diff_index = -1;
                    for (int i = 0; i < length; i++) {
                        if (i != current_index && array[i] > -1) {
                            int diff = abs(array[i] - array[current_index]);
                            if (min_diff == -1 || diff < min_diff) {
                                min_diff = diff;
                                min_diff_index = i;
                            }
                        }
                    }
                    if (min_diff_index > -1) {
                        order_array[order_array_length] = array[min_diff_index];
                        order_array_length++;
                        sum += abs(array[min_diff_index] - array[current_index]);
                        array[current_index] = -1;
                        current_index = min_diff_index;
                    }
                    left--;
                }
                break;
            default:
                break;
        }
        for (int i = 0; i < order_array_length; i++) {
            printf("%d", order_array[i]);
            if (i < order_array_length - 1) {
                printf(",");
            }
        }
        printf("\n");
        printf("%d\n", sum);
        printf("%dT+%dD msec\n", x, y);
    } else {
        printf("Failed to read file\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
