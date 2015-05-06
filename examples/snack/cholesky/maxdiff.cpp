#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int main( int argc, char** argv)
{
    if(argc != 4) {
        printf("Usage : ./a.out <file> <file2> <size>\n");
        return(1);
    }
    FILE* file1 = fopen(argv[1],"r");
    if (file1 == NULL) {
        printf("Illegal file name at first input\n");
        return(1);
    }
    FILE* file2 = fopen(argv[2],"r");
    if (file2 == NULL) {
        printf("Illegal file name at second input\n");
        return(1);
    }

    int N = atoi(argv[3]);
    if (N < 1) {
        printf("Illegal file size\n");
        return(1);
    }
    float max = 0.0;
    int fileIter;
    for (fileIter = 0; fileIter < N; fileIter++) {
        float diff;
        float val1;
        float val2;
        fscanf(file1,"%f",&val1);
        fscanf(file2,"%f",&val2);
        if (val1 < val2) {
            diff = val2 - val1;
        } else {
            diff = val1 - val2;
        }
        //if (diff > max) {
        //    max = diff;
        //} 
        max += diff;
    }
    printf("Max diff = %f\n", max);
}
