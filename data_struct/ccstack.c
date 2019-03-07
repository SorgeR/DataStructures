#include "ccstack.h"
#include "common.h"
#include <malloc.h>
#define CC_EMPTY_STACK -1

static int StCreateNode(CC_STACK_NODE** Node, int Value) 
{
	if(Node==NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	(*Node) = (CC_STACK_NODE*)malloc(sizeof(CC_STACK_NODE));
	if ((*Node) == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	(*Node)->Value = Value;
	(*Node)->Next = NULL;
	

	return CC_SUCCESS;
}

static int StDestroyNode(CC_STACK_NODE **Node) {
	free((*Node));
	Node = NULL;
	return CC_SUCCESS;
}

int StCreate(CC_STACK **Stack)
{
	if (Stack == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	(*Stack) = (CC_STACK*)malloc(sizeof(CC_STACK));
	if ((*Stack) == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	(*Stack)->First = NULL;
	(*Stack)->Dimension = 0;
    
	return CC_SUCCESS;
}

int StDestroy(CC_STACK **Stack)
{
	if (Stack == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	CC_STACK_NODE* iterator = (*Stack)->First;
	if (iterator == NULL) {
		goto cleanup;
	}
	CC_STACK_NODE* auxIterator = iterator->Next;
	while(iterator!=NULL)
	{
		StDestroyNode(&iterator);
		if (auxIterator == NULL)
		{
			goto cleanup;
		}
		else
		{
			iterator = auxIterator;
			auxIterator = auxIterator->Next;
		}
	}

cleanup:
	free(*Stack);
	(*Stack) = NULL;

    return CC_SUCCESS;
}

int StPush(CC_STACK *Stack, int Value)
{
	if (Stack == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	CC_STACK_NODE* newNode;
	if (StCreateNode(&newNode, Value) == CC_SUCCESS)
	{
		newNode->Next = Stack->First;
		Stack->First = newNode;
		Stack->Dimension++;
	}
	else
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
    return CC_SUCCESS;
}

int StPop(CC_STACK *Stack, int *Value)
{
	if (Stack == NULL || Value == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	if (Stack->First == NULL) {
		return CC_EMPTY_STACK;
	}
	else {
		*Value = Stack->First->Value;
		CC_STACK_NODE* firstNode = Stack->First;
		Stack->First= Stack->First->Next;
		StDestroyNode(&firstNode);
		Stack->Dimension--;
	}
    return CC_SUCCESS;
}

int StPeek(CC_STACK *Stack, int *Value)
{
	if (Stack == NULL || Value == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (Stack->First == NULL) {
		return CC_EMPTY_STACK;
	}
	else {
		*Value = Stack->First->Value;
	}

    return CC_SUCCESS;
}

int StIsEmpty(CC_STACK *Stack)
{
	if (Stack == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

    return Stack->First==NULL;
}

int StGetCount(CC_STACK *Stack)
{
	if (Stack == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	
    
    return Stack->Dimension;
}

int StClear(CC_STACK *Stack)
{
	if (Stack == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	int value=0;
	while (StPop(Stack, &value) != CC_EMPTY_STACK);

    return CC_SUCCESS;
}

int StPushStack(CC_STACK *Stack, CC_STACK *StackToPush)
{
	if (Stack == NULL || StackToPush == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	int value=0;
	CC_STACK* auxReverseStack;
	StCreate(&auxReverseStack);
	//getting second stack reversed into an auxiliary stack
	while (!StIsEmpty(StackToPush))
	{
		StPop(StackToPush, &value);
		if(StPush(auxReverseStack, value)==CC_ERROR_MEMORY_ALLOCATION)
		{
			return CC_ERROR_MEMORY_ALLOCATION;
		}
	}
	//getting data from auxiliary to first stack
	while (!StIsEmpty(auxReverseStack))
	{
		StPop(auxReverseStack, &value);
		if (StPush(Stack, value) == CC_ERROR_MEMORY_ALLOCATION)
		{
			return CC_ERROR_MEMORY_ALLOCATION;
		}
	}

	StDestroy(&auxReverseStack);
	
    return CC_SUCCESS;
}
