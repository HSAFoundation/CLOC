#include <iostream>
#include "fibonacci.h"
using namespace std;

extern "C" void sum(int *a, int *b, int *c) {
    *c = *a + *b;
    free(a);
    free(b);
}

/*  Recursive Fibonacci */
void fib(const int n , int *result , atmi_task_t **my_sum_task) {
    If (n < 2) { 
        *result = n; 
        *my_sum_task = NULL;
    } else {
        atmi_task_t *task_sum1;
        atmi_task_t *task_sum2; 
        int *result1 = (int *)malloc(sizeof(int));
        int *result2 = (int *)malloc(sizeof(int));
        fib(n-1,result1,&task_sum1);
        fib(n-2,result2,&task_sum2);
        ATMI_LPARM_CPU(lparm_child); /* Remember ATMI default is asynchronous execution */
        lparm_child->num_requires = 0;
        atmi_task_t *requires[2];
        if (task_sum1 != NULL) {
            requires[lparm_child->num_requires]=task_sum1;
            lparm_child->num_requires +=1;
        }
        if (task_sum2 != NULL) {
            requires[lparm_child->num_requires]=task_sum2;
            lparm_child->num_requires +=1;
        }
        lparm_child->requires = &requires;
        *my_sum_task = sum_cpu(result1,result2,result,lparm_child);
    }
}

int main() {
    const int N = 5;
    int result;

    atmi_task_t *root_sum_task;
    fib(N,&result,&root_sum_task);
    SYNC_TASK(root_sum_task);
    cout << "Fib(" << N << ") = " << result << endl;    
    return 0;
}