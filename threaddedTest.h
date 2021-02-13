#ifndef THREADDED_TEST_HEADER
#define THREADDED_TEST_HEADER
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include "binaryTree.h"

typedef struct {
	FILE *logFile;
	BST *treePointer;
	int *key;
	int *val;
} wrapper_args;

typedef struct {
	int testCases;
	FILE *logFilePointer;
} test_case_stat;

static int testIndex;

volatile static int completedCounter;

void *addNodeWrapper(void *);

void *removeNodeWrapper(void *);

void *fetchNodeWrapper(void *);

void *updateNodeWrapper(void *);

void initTest(test_case_stat *);

void launchTestCase(test_case_stat *, int *, int *, int *, int *);

void populateArrays(int *, int *, int *, int *);

void startTest(test_case_stat *);

#endif