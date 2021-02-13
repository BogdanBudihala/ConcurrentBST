#include "threaddedTest.h"
#include <stdlib.h>

int main(int argc, char *argv[]){
	test_case_stat test;
	
	fprintf(stdout, "Initializing test structure...\n");
	
	initTest(&test);
	
	fprintf(stdout, "Starting testing phase...\n");
	
	startTest(&test);
	
	fprintf(stdout, "Testing completed successfully. Log saved into threadLog.txt\n");
	
	return EXIT_SUCCESS;
}