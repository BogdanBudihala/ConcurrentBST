#include "binaryTree.h"

void initTree(BST *tree){
	if(tree != NULL){
		tree->root = NULL;
		pthread_mutex_init(&tree->mutex, NULL);
	}
	else{
		// Do nothing
	}
}

void destroyTree(BST *tree){
	// To be called only by main thread
	uninitDFS(tree->root);
	pthread_mutex_destroy(&tree->mutex);
}

void uninitDFS(BSTNode *node){
	if(node != NULL){
		uninitDFS(node->left);
		uninitDFS(node->right);
		destroyNode(node);
	}
	else{
		// Do nothing
	}
}

void initNode(BSTNode *node){
	if(node != NULL){
		node->parent = NULL;
		node->left = NULL;
		node->right = NULL;
		pthread_mutex_init(&node->mutex, NULL);
	}
	else{
		// Do nothing
	}
}

void destroyNode(BSTNode *node){
	if(node != NULL){
		pthread_mutex_destroy(&node->mutex);
		free(node);
	}
	else{
		// Do nothing
	}
}

void depthFirstSearch(BSTNode *node, char *buffer, int *size){
	if(node != NULL){
		snprintf(buffer+strlen(buffer), *size - strlen(buffer) >= 0 ? *size - strlen(buffer) : 0, "%d ", node->key);
		
		if(node->left != NULL){
			pthread_mutex_lock(&node->left->mutex);
			depthFirstSearch(node->left, buffer, size);
		}
		
		if(node->right != NULL){
			pthread_mutex_lock(&node->right->mutex);
			depthFirstSearch(node->right, buffer, size);
		}
		
		pthread_mutex_unlock(&node->mutex);
	}
	else{
		// Do nothing
	}
}

void getStringDFS(BST *tree, char *buffer, int *size){
	/*	Get DFS representation of tree. Anything over 'size' will be discarded
		Buffer must handled manually after usage	*/

	pthread_mutex_lock(&tree->mutex);
	
	if(tree->root != NULL){
		pthread_mutex_lock(&tree->root->mutex);
		depthFirstSearch(tree->root, buffer, size);
	}
	else{
		// Do nothing
	}
	
	pthread_mutex_unlock(&tree->mutex);
}


BSTNode *getParent(BST *tree, int *key){
	
	BSTNode *crtNodePointer = NULL, *prevNodePointer = NULL;
	
	if(tree->root != NULL){
		pthread_mutex_lock(&tree->root->mutex);
		
		crtNodePointer = tree->root;
		
		while(crtNodePointer != NULL){
			if(prevNodePointer == NULL){
				pthread_mutex_unlock(&tree->mutex);
			}
			else{
				// Do nothing
			}
			
			if(crtNodePointer->key > *key && crtNodePointer->left == NULL || crtNodePointer->key <= *key && crtNodePointer->right == NULL){
				break;
			}
			else{
				// Do nothing
			}
			
			prevNodePointer = crtNodePointer;
			
			crtNodePointer = crtNodePointer->key > *key ? crtNodePointer->left : crtNodePointer->right;
			
			if(crtNodePointer != NULL){
				pthread_mutex_lock(&crtNodePointer->mutex);
			}
			else{
				// Do nothing
			}
			
			pthread_mutex_unlock(&prevNodePointer->mutex);
		}
	}
	else{
		// Do nothing - New node is going to be the root
	}
	
	return crtNodePointer;
}

int addNode(BST *tree, int *key, int *val){
	/*	Multiple occurrences of the same key are allowed - Will be 
		inserted as right child of the node with the same key	*/
	
	BSTNode *nodeToAddPointer = (BSTNode *)malloc(sizeof(BSTNode));
	
	initNode(nodeToAddPointer);
	
	nodeToAddPointer->key = *key;
	nodeToAddPointer->val = *val;
	
	pthread_mutex_lock(&tree->mutex);

	BSTNode *parentNodePointer = getParent(tree, key);
		
	if(parentNodePointer != NULL){
		nodeToAddPointer->parent = parentNodePointer;
		if(*key < parentNodePointer->key){
			parentNodePointer->left = nodeToAddPointer;
		}
		else{
			parentNodePointer->right = nodeToAddPointer;
		}
			
		pthread_mutex_unlock(&parentNodePointer->mutex);
	}
	else{
		tree->root = nodeToAddPointer;
		pthread_mutex_unlock(&tree->mutex);
	}
	
	return 1;
}

BSTNode *searchTree(BST *tree, int *key){
	// Return first occurrence of the key parameter
	
	BSTNode *crtNodePointer = NULL, *prevNodePointer = NULL;
	
	pthread_mutex_lock(&tree->mutex);
	
	if(tree->root != NULL){
		pthread_mutex_lock(&tree->root->mutex);
		
		crtNodePointer = tree->root;
		
		while(crtNodePointer != NULL){
			
			if(prevNodePointer == NULL){
				pthread_mutex_unlock(&tree->mutex);
			}
			else{
				// Do nothing
			}
			
			if(crtNodePointer->key == *key){
				/*	May lead to segmentation fault if return value is used, 
					but node is deleted right after it is unlocked. 
					Possible fix: Return a pointer to a copy of the found node
					Downside to fix: Requires manual freeing of the allocated copy	*/
					
				pthread_mutex_unlock(&crtNodePointer->mutex);
				break;
			}
			else{
				// Do nothing
			}
			
			prevNodePointer = crtNodePointer;
			
			crtNodePointer = crtNodePointer->key > *key ? crtNodePointer->left : crtNodePointer->right;
			
			if(crtNodePointer != NULL){
				pthread_mutex_lock(&crtNodePointer->mutex);
			}
			else{
				// Do nothing
			}
			
			pthread_mutex_unlock(&prevNodePointer->mutex);
		}	
	}
	else{
		pthread_mutex_unlock(&tree->mutex);
	}
	
	return crtNodePointer;
}

BSTNode *findNodeTwoStepLock(BST *tree, int *key){
	
	BSTNode *crtNodePointer = NULL, *prevNodePointer = NULL;
	
	if(tree->root != NULL){
		pthread_mutex_lock(&tree->root->mutex);
		
		crtNodePointer = tree->root;
		
		while(crtNodePointer != NULL){
			
			if(prevNodePointer != NULL && prevNodePointer->parent == NULL){
				pthread_mutex_unlock(&tree->mutex);
			}
			else if(prevNodePointer != NULL && prevNodePointer->parent != NULL){
				pthread_mutex_unlock(&prevNodePointer->parent->mutex);
			}
			else{
				// Do nothing
			}
			
			if(crtNodePointer->key == *key){
				break;
			}
			else{
				// Do nothing
			}
			
			/*	Fixed bug: Deadlock - Edge case when node does not exist, it 
				does not unlock previously claimed resources	*/
			
			if(crtNodePointer->key > *key && crtNodePointer->left != NULL){
				prevNodePointer = crtNodePointer;
				crtNodePointer = crtNodePointer->left;
				pthread_mutex_lock(&crtNodePointer->mutex);
			}
			else if(crtNodePointer->key <= *key && crtNodePointer->right != NULL){
				prevNodePointer = crtNodePointer;
				crtNodePointer = crtNodePointer->right;
				pthread_mutex_lock(&crtNodePointer->mutex);
			}
			else if(prevNodePointer == NULL){
				pthread_mutex_unlock(&tree->mutex);
				pthread_mutex_unlock(&crtNodePointer->mutex);
				
				// Force exit on next iteration
				crtNodePointer = NULL;
			}
			else{
				pthread_mutex_unlock(&prevNodePointer->mutex);
				pthread_mutex_unlock(&crtNodePointer->mutex);
				
				// Force exit on next iteration
				crtNodePointer = NULL;
			}
			
		}	
	}
	else{
		pthread_mutex_unlock(&tree->mutex);
	}
	 
	return crtNodePointer;
}

BSTNode *getPredecessor(BSTNode *node){
	BSTNode *predecessor = NULL;
	if(node != NULL && node->right != NULL){
		pthread_mutex_lock(&node->right->mutex);
		
		predecessor = getPredecessor(node->right);
		
		if(node->right->right != NULL){
			pthread_mutex_unlock(&node->mutex);
		}
		
	}
	else if(node != NULL && node->right == NULL){
		predecessor = node;
	}
	else{
		// Do nothing - Should not reach this branch
		fprintf(stderr, "Debugging: Else branch of getPredecessor reached!\n");
	}
	return predecessor;
}

int removeNode(BST *tree, int *key){
	/*	Lock parent node as well as current node, as to prevent other 
		threads from descending into the node about to be deleted 
		If node to be deleted is the root itself, the tree itself will be locked	*/
		
	int operationResult = 0;
	
	pthread_mutex_lock(&tree->mutex);
	
	BSTNode *crtNodePointer = findNodeTwoStepLock(tree, key);
	
	if(crtNodePointer != NULL){
		if(crtNodePointer->left != NULL && crtNodePointer->right != NULL){
			/*	Has both children - Find predecessor - Predecessor and its 
				assigned parent will have been locked by procedure
				before it will be modified in the critical section	*/
			
			pthread_mutex_lock(&crtNodePointer->left->mutex);
			
			BSTNode *predecessor = getPredecessor(crtNodePointer->left);
			
			if(predecessor != NULL){
				BSTNode *predecessorParent = predecessor->parent;
				
				// Fixed bug: Edge case - Predecessor is the left child of the node to be deleted
				
				if(predecessor == crtNodePointer->left){
					crtNodePointer->right->parent = predecessor;
					predecessor->right = crtNodePointer->right;
				}
				else{
					crtNodePointer->left->parent = predecessor;
					crtNodePointer->right->parent = predecessor;
					predecessor->left = crtNodePointer->left;
					predecessor->right = crtNodePointer->right;
					predecessorParent->right = NULL;
				}
				
				if(crtNodePointer->parent == NULL){
					predecessor->parent = NULL;
					tree->root = predecessor;
					
					if(predecessor != crtNodePointer->left){
						pthread_mutex_unlock(&predecessorParent->mutex);
					}
					pthread_mutex_unlock(&predecessor->mutex);
					pthread_mutex_unlock(&tree->mutex);
				}
				else{
					// Not root, check whether crtNodePointer is left or right child
					predecessor->parent = crtNodePointer->parent;
					if(crtNodePointer->parent->left == crtNodePointer){
						crtNodePointer->parent->left = predecessor;
					}
					else{
						crtNodePointer->parent->right = predecessor;
					}
					
					if(predecessor != crtNodePointer->left){
						pthread_mutex_unlock(&predecessorParent->mutex);
					}
					pthread_mutex_unlock(&predecessor->mutex);
					pthread_mutex_unlock(&crtNodePointer->parent->mutex);
				}
			}
			else{
				// No predecessor - This branch should not be reached, unexpected behaviour
				fprintf(stderr, "Debugging: Else branch of removeNode reached!\n");
			}
			
			
		}
		else if(crtNodePointer->left != NULL){
			// Has left child - Check for edge case (root)
			pthread_mutex_lock(&crtNodePointer->left->mutex);
			
			if(crtNodePointer->parent == NULL){
				crtNodePointer->left->parent = NULL;
				tree->root = crtNodePointer->left;
				pthread_mutex_unlock(&tree->mutex);
			}
			else{
				// Not root, check whether crtNodePointer is left or right child
				crtNodePointer->left->parent = crtNodePointer->parent;
				if(crtNodePointer->parent->left == crtNodePointer){
					crtNodePointer->parent->left = crtNodePointer->left;
				}
				else{
					crtNodePointer->parent->right = crtNodePointer->left;
				}
				pthread_mutex_unlock(&crtNodePointer->parent->mutex);
			}
			
			pthread_mutex_unlock(&crtNodePointer->left->mutex);
		}
		else if(crtNodePointer->right != NULL){
			// Has right child - Check for edge case (root)
			pthread_mutex_lock(&crtNodePointer->right->mutex);
			
			if(crtNodePointer->parent == NULL){
				crtNodePointer->right->parent = NULL;
				tree->root = crtNodePointer->right;
				pthread_mutex_unlock(&tree->mutex);
			}
			else{
				// Not root, check whether crtNodePointer is left or right child
				crtNodePointer->right->parent = crtNodePointer->parent;
				if(crtNodePointer->parent->left == crtNodePointer){
					crtNodePointer->parent->left = crtNodePointer->right;
				}
				else{
					crtNodePointer->parent->right = crtNodePointer->right;
				}
				pthread_mutex_unlock(&crtNodePointer->parent->mutex);
			}
			
			pthread_mutex_unlock(&crtNodePointer->right->mutex);
		}
		else{
			// Has no children - Check for edge case (root)
			if(crtNodePointer->parent == NULL){
				tree->root = NULL;
				pthread_mutex_unlock(&tree->mutex);
			}
			else{
				// Not root, check whether crtNodePointer is left or right child
				if(crtNodePointer->parent->left == crtNodePointer){
					crtNodePointer->parent->left = NULL;
				}
				else{
					crtNodePointer->parent->right = NULL;
				}
				pthread_mutex_unlock(&crtNodePointer->parent->mutex);
			}
		}
		
		pthread_mutex_unlock(&crtNodePointer->mutex);
		destroyNode(crtNodePointer);
		operationResult = 1;
	}
	else{
		// Do nothing - Node with given key does not exist
	}
	
	return operationResult;
}

int updateNode(BST *tree, int *key, int *value){
	int operationResult = 0;
	
	pthread_mutex_lock(&tree->mutex);
	
	if(tree->root != NULL){
		pthread_mutex_lock(&tree->root->mutex);
		
		BSTNode *crtNodePointer = tree->root, *prevNodePointer = NULL;
		
		while(crtNodePointer != NULL){
			
			if(prevNodePointer == NULL){
				pthread_mutex_unlock(&tree->mutex);
			}
			else{
				// Do nothing
			}
			
			if(crtNodePointer->key == *key){
				crtNodePointer->val = *value;
				pthread_mutex_unlock(&crtNodePointer->mutex);
				
				operationResult = 1;
				break;
			}
			else{
				// Do nothing
			}
			
			prevNodePointer = crtNodePointer;
			
			crtNodePointer = crtNodePointer->key > *key ? crtNodePointer->left : crtNodePointer->right;
			
			if(crtNodePointer != NULL){
				pthread_mutex_lock(&crtNodePointer->mutex);
			}
			else{
				// Do nothing
			}
			
			pthread_mutex_unlock(&prevNodePointer->mutex);
		}	
	}
	else{
		pthread_mutex_unlock(&tree->mutex);
	}
	
	return operationResult;
}


