#include "threaddedTest.h"

void *addNodeWrapper(void *args){
	// Usage: args - logFile, treePointer, key, val
	wrapper_args *wa = (wrapper_args *)args;
	
	int operationResult = addNode(wa->treePointer, wa->key, wa->val);
	
	fprintf(wa->logFile, "\t\t[SUCCESS] Applied addNode on %d \t\t\t %s\n", *wa->key, operationResult == 1 ? "Operation successful" : "No changes made");
	
	free(args);
	
	completedCounter++;
	
	return NULL;
}

void *removeNodeWrapper(void *args){
	// Usage: args - logFile, treePointer, key
	wrapper_args *wa = (wrapper_args *)args;
	
	int operationResult = removeNode(wa->treePointer, wa->key);
	
	fprintf(wa->logFile, "\t\t[SUCCESS] Applied removeNode on %d \t\t %s\n", *wa->key, operationResult == 1 ? "Operation successful" : "No changes made");
	
	free(args);
	
	completedCounter++;
	
	return NULL;
}

void *fetchNodeWrapper(void *args){
	// Usage: args - logFile, treePointer, key
	wrapper_args *wa = (wrapper_args *)args;
	
	void *retVal = (void *)searchTree(wa->treePointer, wa->key);
	
	fprintf(wa->logFile, "\t\t[SUCCESS] Applied searchTree on %d \t\t %s\n", *wa->key, retVal != NULL ? "Operation successful" : "No return value");
	
	free(args);
	
	completedCounter++;
	
	return retVal;
}

void *updateNodeWrapper(void *args){
	// Usage: args - logFile, treePointer, old key, new key
	
	wrapper_args *wa = (wrapper_args *)args;
	
	int operationResult = updateNode(wa->treePointer, wa->key, wa->val);
	
	if(operationResult == 1){
		fprintf(wa->logFile, "\t\t[SUCCESS] Applied updateNode on %d \t\t Operation successful - New value: %d\n", *wa->key, *wa->val);
	}
	else{
		fprintf(wa->logFile, "\t\t[SUCCESS] Applied updateNode on %d \t\t No changes made\n", *wa->key);
	}
	
	
	
	free(args);
	
	completedCounter++;
	
	return NULL;
}

void populateArrays(int *arraySize, int *keys, int *values, int *newValues){
	// Keys are not guaranteed to have been generated uniquely
	
	for(int i=0; i < *arraySize; i++){
		*(keys+i) = rand() % 20;
		*(values+i) = rand() % 50000;
		*(newValues+i) = rand() % 50000;
	}
}

void launchTestCase(test_case_stat *test, int *arraySize, int *keys, int *values, int *newValues){
	// One thread per operation
	
	BST tree;
	pthread_t threads[*arraySize];
	int threadTaskIndex = 0, outputBufferSize = 4096;
	void *(*operation[4])(void *) = {addNodeWrapper, removeNodeWrapper, fetchNodeWrapper, updateNodeWrapper};
	char outputBuffer[outputBufferSize];
	
	memset(outputBuffer, 0, outputBufferSize);
	
	initTree(&tree);
	fprintf(test->logFilePointer, "\n[Case %d/%d] - %d operations\n\n", testIndex+1, test->testCases, *arraySize);
	
	for(int i=0; i < *arraySize; i++){
		wrapper_args *argStruct = (wrapper_args *)malloc(sizeof(wrapper_args));
		
		argStruct->logFile = test->logFilePointer;
		argStruct->treePointer = &tree;
		argStruct->key = keys+i;
		
		
		threadTaskIndex = rand()%4;
		
		if(threadTaskIndex == 3){
			argStruct->val = newValues+i;
		}
		else{
			argStruct->val = values+i;
		}
		
		fprintf(test->logFilePointer, "\t\t[] Applying %s on %d\n", threadTaskIndex == 0 ? "addNode" : 
				threadTaskIndex == 1 ? "removeNode" : threadTaskIndex == 2 ? "searchTree" : "updateNode", *(keys+i));
		
		if(pthread_create(threads+i, NULL, *(operation+threadTaskIndex), argStruct) == -1){
			fprintf(stderr, "Error: Unable to create thread\n");
            free(argStruct);
        }
		else{
			// Do nothing
		}
	}
	
	for(int i=0; i < *arraySize; i++){
		if(pthread_join(*(threads+i), NULL) == -1) {
			fprintf(stderr, "Error: Unable to join thread\n");
        }
	}
	
	getStringDFS(&tree, outputBuffer, &outputBufferSize);
	
	fprintf(test->logFilePointer, "\n\t\tNode keys DFS Representation: %s\n", outputBuffer);
	
	destroyTree(&tree);
	
	fprintf(test->logFilePointer, "\n[Case %d/%d] %s\n", testIndex+1, test->testCases, completedCounter == *arraySize ? "passed" : "failed");
}

void startTest(test_case_stat *test){
	int arraySize = 0;
	int *keys, *values, *newValues;
	
	fprintf(test->logFilePointer, "\t\t****** Ran %d test cases ******\n", test->testCases);
	
	for(testIndex = 0; testIndex < test->testCases; testIndex++){
		completedCounter = 0;
		arraySize = rand()%30;
		
		keys = (int *)malloc(sizeof(int)*arraySize);
		values = (int *)malloc(sizeof(int)*arraySize);
		newValues = (int *)malloc(sizeof(int)*arraySize);
		
		populateArrays(&arraySize, keys, values, newValues);
		
		launchTestCase(test, &arraySize, keys, values, newValues);
		
		free(keys);
		free(values);
		free(newValues);
	}
}

void initTest(test_case_stat *test){
	// Should be called only once
	srand(time(NULL));
	test->testCases = 1 + rand()%50;
	test->logFilePointer = fopen("threadLog.txt", "w");
	
	if(test->logFilePointer == NULL){
		fprintf(stderr, "Could not open threadLog.txt. Aborting testing\n");
		exit(EXIT_FAILURE);
	}
}

