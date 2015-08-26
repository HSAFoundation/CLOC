#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;
#include "test_hsail_lib.h"
static const int NUMELEMENTS = 40;
float *inArray = new float[NUMELEMENTS];
float *outArray = new float[NUMELEMENTS];
int main(int argc, char *argv[]) {
	// initialize inArray
	for (int i=0; i<NUMELEMENTS; i++) {inArray[i] =  1.0 / ((float)i + 1.0); }
	
        SNK_INIT_LPARM(lparm,NUMELEMENTS);
        testkernel(outArray,inArray,lparm);
	bool passed = true;
	for (int i=0; i<NUMELEMENTS; i++) {
		cout << i << "->" << outArray[i] << "," << sin(inArray[i]) << ",  ";
		if ( (outArray[i]-sin(inArray[i])) > 1.0 ) passed = false;
	}
 	cout << endl << (passed ? "PASSED" : "FAILED") << endl;
	return 0;
}
