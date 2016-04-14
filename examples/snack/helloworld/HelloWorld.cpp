#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include "hw.h"
int main(int argc, char* argv[]) {
	const char* input1 = "Gdkkn\x1FGR@\x1FVnqkc";
	size_t strlength = strlen(input1);
	char *input = (char*) malloc_global(strlength + 1);
	char *output = (char*) malloc_global(strlength + 1);
	memcpy(input, input1, strlength+1);
        SNK_INIT_LPARM(lparm,strlength);
        decode(input,output,lparm);
	output[strlength] = '\0';
	cout << output << endl;
	free_global(input);
	free_global(output);
	return 0;

}
