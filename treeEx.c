#include <stdlib.h>
#include <stdio.h>

struct Node
{
	int data_;
	struct Node* leftPtr_;
	struct Node* rightPtr_;
};

typedef struct Node NodeType;

void infix (NodeType* ptr)
{
	if (ptr == NULL) 
		return;
	infix(ptr->leftPtr_);
	printf("%d ", ptr->data_);
	infix(ptr->rightPtr_);
}

void prefix (NodeType* ptr)
{
	if (ptr == NULL) 
		return;
	printf("%d ", ptr->data_);
	infix(ptr->leftPtr_);
	infix(ptr->rightPtr_);
}

void postfix (NodeType* ptr)
{
	if (ptr == NULL) 
		return;
	infix(ptr->leftPtr_);
	infix(ptr->rightPtr_);
	printf("%d ", ptr->data_);
}

void recursivelyFree (NodeType* ptr)
{
	if (ptr == NULL)
		return;
	recursivelyFree(ptr->leftPtr_);
	recursivelyFree(ptr->rightPtr_);
	free(ptr);
}

int main()
{
	struct Node* rootPtr = (NodeType*)malloc(sizeof(NodeType));
	
	rootPtr->data_ = 5;
	rootPtr->leftPtr_ = (NodeType*)malloc(sizeof(NodeType));
	rootPtr->rightPtr_ = (NodeType*)malloc(sizeof(NodeType));
	rootPtr->leftPtr_->data_ = 3;
	rootPtr->leftPtr_->leftPtr_ = (NodeType*)malloc(sizeof(NodeType));
	rootPtr->leftPtr_->leftPtr_->data_ = 2;
	rootPtr->leftPtr_->leftPtr_->leftPtr_ = NULL;
	rootPtr->leftPtr_->leftPtr_->rightPtr_ = NULL;
	rootPtr->leftPtr_->rightPtr_ = (NodeType*)malloc(sizeof(NodeType));
	rootPtr->leftPtr_->rightPtr_->data_ = 4;
	rootPtr->leftPtr_->rightPtr_->leftPtr_ = NULL;
	rootPtr->leftPtr_->rightPtr_->rightPtr_ =NULL;
	rootPtr->rightPtr_->data_ = 7;
	rootPtr->rightPtr_->leftPtr_ = (NodeType*)malloc(sizeof(NodeType));
	rootPtr->rightPtr_->leftPtr_->data_ = 6;
	rootPtr->rightPtr_->leftPtr_->leftPtr_ = NULL;
	rootPtr->rightPtr_->leftPtr_->rightPtr_ = NULL;
	rootPtr->rightPtr_->rightPtr_ = (NodeType*)malloc(sizeof(NodeType));
	rootPtr->rightPtr_->rightPtr_->data_ = 8;
	rootPtr->rightPtr_->rightPtr_->leftPtr_ = NULL;
	rootPtr->rightPtr_->rightPtr_->rightPtr_ = NULL;
	puts("infix order:");
	infix(rootPtr);
	puts("\nprefix order:");
	prefix(rootPtr);
	puts("\npostfix order:");
	postfix(rootPtr);
	recursivelyFree(rootPtr);
	return(EXIT_SUCCESS);
}