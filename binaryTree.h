#ifndef BINARY_TREE_HEADER
#define BINARY_TREE_HEADER
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

struct node{
	struct node *left;
	struct node *right;
	struct node *parent;
	pthread_mutex_t mutex;
	int key;
	int val;
};

typedef struct node BSTNode;

struct tree{
	BSTNode *root;
	pthread_mutex_t mutex;
};

typedef struct tree BST;

void initTree(BST *);

void destroyTree(BST *);

void destroyNode(BSTNode *);

void uninitDFS(BSTNode *);

int addNode(BST *, int *, int *);

int removeNode(BST *, int *);

BSTNode *searchTree(BST *, int *);

int updateNode(BST *, int *, int *);

BSTNode *getParent(BST *, int *);

BSTNode *getPredecessor(BSTNode *);

BSTNode *findNodeTwoStepLock(BST *, int *);

void initNode(BSTNode *);

void getStringDFS(BST *, char *buffer, int *);

void depthFirstSearch(BSTNode *, char *, int *);

#endif