#include <stdlib.h>
#include <stdio.h>

#define ARR(x) arr[x]
//#define G(0) 1
//#define G(1) 2
//
//#define G(x) 2
//
int main() {
int arr[3] = {1, 2, 3};
char arr[3][3] = {"xxx", "yyy", "zzz"};
printf("2 = %d\n",ARR(1));
printf("3 = %d\n",ARR(2));

//printf("1 = %d\n",G(0));
//printf("2 = %d\n",G(1));
}
