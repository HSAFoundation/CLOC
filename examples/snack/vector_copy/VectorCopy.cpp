#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <libelf.h>
#include <iostream>
#include "vector_copy.h"

int main(int argc, char **argv)
{

	//Setup kernel arguments
	int* in=(int*)malloc(1024*1024*4);
	int* out=(int*)malloc(1024*1024*4);
	memset(out, -1, 1024*1024*4);
	for(int i=0; i<1024*1024; i++)
		in[i] = i;
	
	Launch_params_t lparm={ .ndim=1, .gdims={1024*1024}, .ldims={256} };
	vcopy(in, out, lparm);

	//Validate
	bool valid=true;
	int failIndex=0;
	for(int i=0; i<1024*1024; i++) 
	{
		int in_i = in[i];
		int out_i = out[i];
		if(out_i!=in_i) {
			failIndex=i;
			valid=false;
			break;
		}
	}
	if(valid)
		printf("passed validation\n");
	else 
		printf("VALIDATION FAILED!\nBad index: %d\n", failIndex);


	free(in);
	free(out);

    return 0;
}
